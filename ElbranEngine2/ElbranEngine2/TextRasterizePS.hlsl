#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float4 color;
}

// must match Font.cpp
struct BezierCurve {
	float2 start;
	float2 control;
	float2 end;
};

StructuredBuffer<BezierCurve> curves : register(t0);
Buffer<uint> glyphStartIndices : register(t1);

#define Is0To1(num) (1 - saturate(ceil(abs(saturate(num) - num))))
#define CalcBezierX(curve, t) lerp(lerp(curve.start.x, curve.control.x, t), lerp(curve.control.x, curve.end.x, t), t)
#define CalcBezierY(curve, t) lerp(lerp(curve.start.y, curve.control.y, t), lerp(curve.control.y, curve.end.y, t), t)
#define SCALE_HELPER 100 // avoid floating-point math with tiny numbers

float4 main(VertexToPixel input) : SV_TARGET {
	int glyphIndex = (int)input.uv.x;
	float2 localCoords = frac(input.uv);
	localCoords.y = 1.0 - localCoords.y; // flip upside-down to match UV orientation
	
	uint startCurve = glyphStartIndices[glyphIndex];
	uint lastCurve = glyphStartIndices[glyphIndex + 1] - 1;
	int windCount = 0; // add 1 for an intersection of a downward curve and -1 for an upward curve
	
	// find distances from pixel center to closest curve in each direction
	float pixelWidth = ddx(input.uv.x);
	float rightDist = pixelWidth * 0.5f;
	float leftDist = rightDist;
	float upDist = rightDist;
	float downDist = rightDist;
	
	for(uint curveIndex = startCurve; curveIndex <= lastCurve; curveIndex++) {
		BezierCurve curve = curves[curveIndex];
		
		// -- test horizontally first --
		
		// use quadratic formula to find times where the curve is intersected
		float2 a = (curve.start - 2 * curve.control + curve.end);
		float2 b = 2 * (curve.control - curve.start);
		float2 c = curve.start - localCoords;
	
		// determine if this is a horizontal line, slanted line, or curve
		float2 isLine = (abs(a) < 0.00001);
		float2 isCurve = 1.0 - isLine;
		isLine *= (abs(b) > 0.00001); // omit horizontal lines
		
		float2 determinant = b*b - 4*a*c;
		float2 onCurve = isCurve * saturate(ceil(determinant)); // 1 when determinant > 0, 0 otherwise
		float2 rootDeterminant = sqrt(max(determinant, 0));
		float2 safeA = a + (a == 0.0); // avoid divide by 0
		float2 safeB = b + (b == 0.0);
		
		float2 t1 = onCurve * (-b + rootDeterminant) / (2.0*safeA) 
				+ isLine * (-c / safeB); // simpler equation for straight lines
		float2 t2 = onCurve * (-b - rootDeterminant) / (2.0*safeA);
		
		float slopeY1 = onCurve.y * (2 * a.y * t1.y) + b.y;
		float slopeY2 = 2*a.y*t2.y + b.y; // unused for straight lines
		
		float bezierX = (a.x * t1.y * t1.y) + (b.x * t1.y) + c.x;
		int hasIntersection = (onCurve.y + isLine.y) * Is0To1(t1.y);
		//rightDist = hasIntersection * (bezierDiff > 0) * (min(rightDist, bezierDiff) - rightDist) + rightDist;
		//leftDist = hasIntersection * (bezierDiff < 0) * (min(leftDist, -bezierDiff) - leftDist) + leftDist;
		windCount += hasIntersection * (bezierX >= 0.0) * sign(slopeY1);
		
		bezierX = (a.x * t2.y * t2.y) + (b.x * t2.y) + c.x;
		hasIntersection = onCurve.y * Is0To1(t2.y); // straight lines have 1 intersection
		//rightDist = hasIntersection * (bezierDiff > 0) * (min(rightDist, bezierDiff) - rightDist) + rightDist;
		//leftDist = hasIntersection * (bezierDiff < 0) * (min(leftDist, -bezierDiff) - leftDist) + leftDist;
		windCount += hasIntersection * (bezierX >= 0.0) * sign(slopeY2); // for some reason, need to recalculate bezierX
		
		// -- test vertically second --
		/*a = (curve.start.x - 2 * curve.control.x + curve.end.x);
		b = 2 * (curve.control.x - curve.start.x);
		c = curve.start.x - localCoords.x;
		
		determinant = b*b - 4*a*c;
		onCurve = isCurve * saturate(ceil(determinant));
		rootDeterminant = sqrt(max(determinant, 0));
		a += (a == 0.0);
		safeB = b + (b == 0.0);
		
		t1 = onCurve * (-b + rootDeterminant) / (2.0*safeA) 
				+ isLine * (-c / safeB); // simpler equation for straight lines
		t2 = onCurve * (-b - rootDeterminant) / (2.0*safeA);
		
		bezierDiff = CalcBezierY(curve, t1) - localCoords.y;
		hasIntersection = (onCurve + isLine) * Is0To1(t1);
		upDist = hasIntersection * (bezierDiff > 0) * (min(upDist, bezierDiff) - upDist) + upDist;
		downDist = hasIntersection * (bezierDiff < 0) * (min(downDist, -bezierDiff) - downDist) + downDist;
		
		bezierDiff = CalcBezierY(curve, t2) - localCoords.y;
		hasIntersection = (onCurve + isLine) * Is0To1(t2);
		upDist = hasIntersection * (bezierDiff > 0) * (min(upDist, bezierDiff) - upDist) + upDist;
		downDist = hasIntersection * (bezierDiff < 0) * (min(downDist, -bezierDiff) - downDist) + downDist;*/
	}
	
	float4 result = color;
	int isInGlyph = windCount != 0;
	float coverArea = 1;//(rightDist + leftDist) * (upDist + downDist) / (pixelWidth * pixelWidth);
	result.a = isInGlyph;// * (coverArea) + (1 - isInGlyph) * (1.0 - coverArea);
	//result.r = rightDist / pixelWidth * 2.0;
	clip(result.a - 0.00001);
	return result;
}