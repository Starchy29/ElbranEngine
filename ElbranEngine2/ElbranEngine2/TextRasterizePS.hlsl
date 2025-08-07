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

float4 main(VertexToPixel input) : SV_TARGET {
	int glyphIndex = (int)input.uv.x;
	float2 localCoords = frac(input.uv);
	localCoords.y = 1.0 - localCoords.y; // flip upside-down to match UV orientation
	
	uint startCurve = glyphStartIndices[glyphIndex];
	uint lastCurve = glyphStartIndices[glyphIndex + 1] - 1;
	int windCount = 0; // add 1 for an intersection of an upward curve and -1 for a downward curve
	
	// find distances from pixel center to closest curve in each direction
	float rightDist = 1.0;
	float leftDist = 1.0;
	float upDist = 1.0;
	float downDist = 1.0;
	
	for(uint curveIndex = startCurve; curveIndex <= lastCurve; curveIndex++) {
		BezierCurve curve = curves[curveIndex];
		
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
		
		// test horizontally
		float slopeY1 = onCurve.y * (2 * a.y * t1.y) + b.y;
		float slopeY2 = 2 * a.y * t2.y + b.y; // unused for straight lines
		
		float bezierDiff = (a.x * t1.y * t1.y) + (b.x * t1.y) + c.x; // c was already shifted by the local position, making this relative to the pixel
		int hasIntersection = (onCurve.y + isLine.y) * (t1.y >= 0.0) * (t1.y <= 1.0);
		rightDist = hasIntersection * (bezierDiff > 0.0) * (min(rightDist, bezierDiff) - rightDist) + rightDist;
		leftDist = hasIntersection * (bezierDiff < 0.0) * (min(leftDist, -bezierDiff) - leftDist) + leftDist;
		windCount += hasIntersection * (bezierDiff >= 0.0) * sign(slopeY1);
		
		bezierDiff = (a.x * t2.y * t2.y) + (b.x * t2.y) + c.x;
		hasIntersection = onCurve.y * (t2.y >= 0.0) * (t2.y <= 1.0); // straight lines have 1 intersection
		rightDist = hasIntersection * (bezierDiff > 0.0) * (min(rightDist, bezierDiff) - rightDist) + rightDist;
		leftDist = hasIntersection * (bezierDiff < 0.0) * (min(leftDist, -bezierDiff) - leftDist) + leftDist;
		windCount += hasIntersection * (bezierDiff >= 0.0) * sign(slopeY2);
	
		// test vertically
		bezierDiff = (a.y * t1.x * t1.x) + (b.y * t1.x) + c.y; 
		hasIntersection = (onCurve.x + isLine.x) * (t1.x >= 0.0) * (t1.x <= 1.0);
		upDist = hasIntersection * (bezierDiff > 0.0) * (min(upDist, bezierDiff) - upDist) + upDist;
		downDist = hasIntersection * (bezierDiff < 0.0) * (min(downDist, -bezierDiff) - downDist) + downDist;
		
		bezierDiff = (a.y * t2.x * t2.x) + (b.y * t2.x) + c.y;
		hasIntersection = onCurve.x * (t2.x >= 0.0) * (t2.x <= 1.0);
		upDist = hasIntersection * (bezierDiff > 0.0) * (min(upDist, bezierDiff) - upDist) + upDist;
		downDist = hasIntersection * (bezierDiff < 0.0) * (min(downDist, -bezierDiff) - downDist) + downDist;
	}
	
	float pixelWidth = 0.02;
	float inversePixel = 1.0 / pixelWidth;
	rightDist = min(rightDist * inversePixel, 0.5);
	leftDist = min(leftDist * inversePixel, 0.5);
	upDist = min(upDist * inversePixel, 0.5);
	downDist = min(downDist * inversePixel, 0.5);
	float coverArea = ((rightDist + leftDist) + (upDist + downDist)) * 0.5f;
	
	int isInGlyph = windCount != 0;
	
	float4 result = color;
	result.a = isInGlyph * coverArea + (1 - isInGlyph) * (1.0 - coverArea);
	clip(result.a - 0.00001);
	return result;
}