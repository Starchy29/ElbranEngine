#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float4 color;
}

// must match Font.cpp
struct BezierCurve {
	float2 start;
	float2 end;
	float2 control;
};

StructuredBuffer<BezierCurve> curves : register(t0);
Buffer<uint> glyphStartIndices : register(t1);

#define Is0To1(num) (1 - saturate(ceil(abs(saturate(num) - num))))
#define CalcBezierX(curve, t) lerp(lerp(curve.start.x, curve.control.x, t), lerp(curve.control.x, curve.end.x, t), t)

float4 main(VertexToPixel input) : SV_TARGET {
	int glyphIndex = (int)input.uv.x;
	float2 localCoords = frac(input.uv);
	localCoords.y = 1.0 - localCoords.y; // flip upside-down to match UV orientation
	
	// find all curve intersection for this row of pixels
	uint intersections = 0;
	uint startCurve = glyphStartIndices[glyphIndex];
	uint lastCurve = glyphStartIndices[glyphIndex + 1] - 1;
	
	for(uint curveIndex = startCurve; curveIndex <= lastCurve; curveIndex++) {
		// use quadratic formula to find times where the curve is intersected
		float a = (curves[curveIndex].start.y - 2 * curves[curveIndex].control.y + curves[curveIndex].end.y);
		float b = 2 * (curves[curveIndex].control.y - curves[curveIndex].start.y);
		float c = curves[curveIndex].start.y - localCoords.y;
	
		// determine if this is a horizontal line, slanted line, or curve
		float isLine = (abs(a) < 0.00001);
		float isCurve = 1.0 - isLine;
		isLine *= (abs(b) > 0.00001); // omit horizontal lines
		
		float determinant = b*b - 4*a*c;
		float rootDeterminant = sqrt(max(determinant, 0));
		a += (a == 0.0); // avoid divide by 0
		float safeB = b + (b == 0.0);
		
		float t1 = isCurve * (-b + rootDeterminant) / (2.0*a) 
				+ isLine * (-c / safeB); // simpler equation for straight lines
		float t2 = isCurve * (-b - rootDeterminant) / (2.0*a);
		
		float onCurve = isCurve * saturate(ceil(determinant)); // 1 when determinant > 0, 0 otherwise
		uint hasIntersection = (onCurve + isLine) * Is0To1(t1) * (CalcBezierX(curves[curveIndex], t1) >= localCoords.x);
		intersections += hasIntersection;
		
		hasIntersection = onCurve * Is0To1(t2) * (CalcBezierX(curves[curveIndex], t2) >= localCoords.x); // straight lines have 1 intersection
		intersections += hasIntersection;
	}
	
	float4 result = color * (intersections % 2 == 1);
	//clip(result.a - 0.00001);
	return result;
}