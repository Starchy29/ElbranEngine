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

#define EPSILON 1e-5

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
	
	// to fix anti-aliasing on overlapped contours, note the wind direction of each crossed curve inside the pixel
	float rightWind = 0.0;
	float leftWind = 0.0;
	float upWind = 0.0;
	float downWind = 0.0;
	
	for(uint curveIndex = startCurve; curveIndex <= lastCurve; curveIndex++) {
		BezierCurve curve = curves[curveIndex];
		
		// use quadratic formula to find times where the curve is intersected
		float2 a = (curve.start - 2 * curve.control + curve.end);
		float2 b = 2 * (curve.control - curve.start);
		float2 c = curve.start - localCoords;
	
		// determine if this is a horizontal line, slanted line, or curve
		float2 isLine = (abs(a) < EPSILON);
		float2 isCurve = 1.0 - isLine;
		isLine *= (abs(b) > EPSILON); // omit horizontal lines
		
		float2 determinant = b*b - 4*a*c;
		float2 onCurve = isCurve * (determinant > -EPSILON); 
		float2 rootDeterminant = sqrt(max(determinant, 0));
		float2 safeA = a + (a == 0.0); // avoid divide by 0
		float2 safeB = b + (b == 0.0);
		
		float2 t1 = onCurve * (-b + rootDeterminant) / (2.0*safeA) 
					+ isLine * (-c / safeB); // simpler equation for straight lines
		float2 t2 = onCurve * (-b - rootDeterminant) / (2.0*safeA);
		
		float2 wind1 = onCurve + isLine * sign(b);
		// float2 wind2 = -onCurve + isLine * sign(b); // since this is unused for straight lines, it's always -1
		
		if((onCurve.y || isLine.y) && (wind1.y == 0.0)) return float4(1, 0, 0, 1);
		
		float lastRight = rightDist;
		float lastLeft = leftDist;
		float lastUp = upDist;
		float lastDown = downDist;
		
		// test horizontally	
		float bezierDiff = (a.x * t1.y * t1.y) + (b.x * t1.y) + c.x; // c was already shifted by the local position, making this relative to the pixel
		int hasIntersection = (onCurve.y + isLine.y) * (t1.y > -EPSILON) * (t1.y <= 1.0 + EPSILON);
		windCount += hasIntersection * (bezierDiff >= 0.0) * wind1.y;
		rightDist = hasIntersection * (bezierDiff > 0.0) * (min(rightDist, bezierDiff) - rightDist) + rightDist;
		rightWind = (rightDist != lastRight) * (wind1.y - rightWind) + rightWind;
		leftDist = hasIntersection * (bezierDiff < 0.0) * (min(leftDist, -bezierDiff) - leftDist) + leftDist;
		leftWind = (leftDist != lastLeft) * (wind1.y - leftWind) + leftWind;
		
		lastRight = rightDist;
		lastLeft = leftDist;
		
		bezierDiff = (a.x * t2.y * t2.y) + (b.x * t2.y) + c.x;
		hasIntersection = onCurve.y * (t2.y > -EPSILON) * (t2.y <= 1.0 + EPSILON); // straight lines have 1 intersection
		windCount -= hasIntersection * (bezierDiff >= 0.0);
		rightDist = hasIntersection * (bezierDiff > 0.0) * (min(rightDist, bezierDiff) - rightDist) + rightDist;
		rightWind = (rightDist != lastRight) * (-1.0 - rightWind) + rightWind;
		leftDist = hasIntersection * (bezierDiff < 0.0) * (min(leftDist, -bezierDiff) - leftDist) + leftDist;
		leftWind = (leftDist != lastLeft) * (-1.0 - leftWind) + leftWind;
	
		// test vertically
		bezierDiff = (a.y * t1.x * t1.x) + (b.y * t1.x) + c.y; 
		hasIntersection = (onCurve.x + isLine.x) * (t1.x > -EPSILON) * (t1.x <= 1.0 + EPSILON);
		upDist = hasIntersection * (bezierDiff > 0.0) * (min(upDist, bezierDiff) - upDist) + upDist;
		upWind = (upDist != lastUp) * (wind1.x - upWind) + upWind;
		downDist = hasIntersection * (bezierDiff < 0.0) * (min(downDist, -bezierDiff) - downDist) + downDist;
		downWind = (downDist != lastDown) * (wind1.x - downWind) + downWind;
		
		lastUp = upDist;
		lastDown = downDist;
		
		bezierDiff = (a.y * t2.x * t2.x) + (b.y * t2.x) + c.y;
		hasIntersection = onCurve.x * (t2.x > -EPSILON) * (t2.x <= 1.0 + EPSILON);
		upDist = hasIntersection * (bezierDiff > 0.0) * (min(upDist, bezierDiff) - upDist) + upDist;
		upWind = (upDist != lastUp) * (-1.0 - upWind) + upWind;
		downDist = hasIntersection * (bezierDiff < 0.0) * (min(downDist, -bezierDiff) - downDist) + downDist;
		downWind = (downDist != lastDown) * (-1.0 - downWind) + downWind;
	}
	
	int isInGlyph = windCount != 0;
	
	const float pixelWidth = ddx(input.uv.x);
	float inversePixel = 1.0 / pixelWidth;
	rightDist = min(rightDist * inversePixel, 0.5);
	leftDist = min(leftDist * inversePixel, 0.5);
	upDist = min(upDist * inversePixel, 0.5);
	downDist = min(downDist * inversePixel, 0.5);
	
	// fix antialiasing on overlapped contours
	float insideOverlap = abs(windCount) > 1;
	rightDist = saturate(insideOverlap + isInGlyph * (rightWind == 1)) * (0.5 - rightDist) + rightDist;
	leftDist = saturate(insideOverlap + isInGlyph * (leftWind == -1)) * (0.5 - leftDist) + leftDist;
	upDist = saturate(insideOverlap + isInGlyph * (upWind == -1)) * (0.5 - upDist) + upDist;
	downDist = saturate(insideOverlap + isInGlyph * (downWind == 1)) * (0.5 - downDist) + downDist;
	
	float coverArea = ((rightDist + leftDist) + (upDist + downDist)) * 0.5;
	
	float4 result = color;
	result.a = isInGlyph * coverArea + (1 - isInGlyph) * (1.0 - coverArea);
	clip(result.a - 0.00001);
	return result;
}