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

float IsInGlyph(float2 pos, uint startCurve, uint lastCurve);

float4 main(VertexToPixel input) : SV_TARGET {
	int glyphIndex = (int)input.uv.x;
	float2 localCoords = frac(input.uv);
	localCoords.y = 1.0 - localCoords.y; // flip upside-down to match UV orientation
	
	uint startCurve = glyphStartIndices[glyphIndex];
	uint lastCurve = glyphStartIndices[glyphIndex + 1] - 1;
	
	float pixelFourth = ddx(input.uv.x) / 4.0;
	float numInGlyph = 0.0;
	numInGlyph += IsInGlyph(localCoords + float2(pixelFourth, pixelFourth), startCurve, lastCurve);
	numInGlyph += IsInGlyph(localCoords + float2(-pixelFourth, pixelFourth), startCurve, lastCurve);
	numInGlyph += IsInGlyph(localCoords + float2(pixelFourth, -pixelFourth), startCurve, lastCurve);
	numInGlyph += IsInGlyph(localCoords + float2(-pixelFourth, -pixelFourth), startCurve, lastCurve);
	
	float4 result = color;
	result.a = numInGlyph / 4.0;
	clip(result.a - 0.00001);
	return result;
}

float IsInGlyph(float2 pos, uint startCurve, uint lastCurve) {
	// find all curve intersection for this row of pixels
	uint windCount = 0; // add 1 for an intersection of a downward curve and -1 for an upward curve
	
	for(uint curveIndex = startCurve; curveIndex <= lastCurve; curveIndex++) {
		// use quadratic formula to find times where the curve is intersected
		float a = (curves[curveIndex].start.y - 2 * curves[curveIndex].control.y + curves[curveIndex].end.y);
		float b = 2 * (curves[curveIndex].control.y - curves[curveIndex].start.y);
		float c = curves[curveIndex].start.y - pos.y;
	
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
		
		float slopeY1 = isCurve * (2*a*t1) + b;
		float slopeY2 = 2*a*t2 + b; // unused for straight lines
		
		float onCurve = isCurve * saturate(ceil(determinant)); // 1 when determinant > 0, 0 otherwise
		uint hasIntersection = (onCurve + isLine) * Is0To1(t1) * (CalcBezierX(curves[curveIndex], t1) >= pos.x);
		windCount += hasIntersection * sign(slopeY1);
		
		hasIntersection = onCurve * Is0To1(t2) * (CalcBezierX(curves[curveIndex], t2) >= pos.x); // straight lines have 1 intersection
		windCount += hasIntersection * sign(slopeY2);
	}
	
	return windCount != 0;
}