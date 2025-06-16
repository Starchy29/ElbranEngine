
// must match Font.cpp
struct BezierCurve {
	float2 start;
	float2 end;
	float2 control;
};

cbuffer Constants : register(b0) {
	uint numGlyphs;
	uint glyphSize; // pixel width of square for each glyph
	uint rowsCols; // rows and cols are equal
}

RWTexture2D<float4> outAtlas : register(u0);
StructuredBuffer<BezierCurve> curves : register(t0);
Buffer<uint> glyphStartIndices : register(t1);

#define CalcBezierX(curve, t) lerp(lerp(curve.start.x, curve.control.x, t), lerp(curve.control.x, curve.end.x, t), t)
#define Is0To1(num) (1 - saturate(ceil(abs(saturate(num) - num))))

[numthreads(4, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	// x is atlas column, y is pixel coordinate
	if(DTid.x >= rowsCols || DTid.y >= rowsCols * glyphSize) return;
	
	uint localY = DTid.y % glyphSize;
	
	// determine glyph index
	uint2 atlasPos = uint2(DTid.x, DTid.y / glyphSize);
	uint glyphIndex = rowsCols * atlasPos.y + atlasPos.x;
	if(glyphIndex >= numGlyphs) return;
	
	// find all curve intersection for this row of pixels
	float intersections[32];
	uint intersectCount = 1; // index 0 is reserved to allow a trick that avoids branching
	
	uint startCurve = glyphStartIndices[glyphIndex];
	uint lastCurve = glyphStartIndices[glyphIndex + 1] - 1;
	float testY = 1.0 - frac((float)DTid.y / glyphSize); // flip upside-down to match UV orientation
	for(uint curveIndex = startCurve; curveIndex <= lastCurve; curveIndex++) {
		// use quadratic formula to find times where the curve is intersected
		float a = (curves[curveIndex].start.y - 2 * curves[curveIndex].control.y + curves[curveIndex].end.y);
		float b = 2 * (curves[curveIndex].control.y - curves[curveIndex].start.y);
		float c = curves[curveIndex].start.y - testY;
	
		// determine if this is a horizontal line, slanted line, or curve
		float isLine = abs(a) < 0.00001;
		float isHorizontal = isLine * (abs(b) < 0.00001);
		isLine -= isHorizontal;
		float isCurve = 1.0 - isLine - isHorizontal;
		
		float determinant = b*b - 4*a*c;
		float rootDeterminant = sqrt(max(determinant, 0));
		a += (a == 0.0); // avoid divide by 0
		b += (b == 0.0);
		
		float t1 = isCurve * (-b + rootDeterminant) / (2.0*a) 
				+ isLine * (-c / b); // simpler equation for straight lines
				+ isHorizontal; // add end point at t=1 for horizontal line
		float t2 = isCurve * (-b - rootDeterminant) / (2.0*a); // when horizontal line, this will be t=0, the other end point
		
		float onCurve = isCurve * saturate(ceil(determinant)); // 1 when determinant > 0, 0 otherwise
		float hasIntersection = (onCurve + isLine + isHorizontal) * Is0To1(t1);
		intersections[hasIntersection * intersectCount] = CalcBezierX(curves[curveIndex], t1);
		intersectCount += 1 * hasIntersection;
		
		hasIntersection = (onCurve + isHorizontal) * Is0To1(t2); // non-horizontal straight lines have 1 intersection
		intersections[hasIntersection * intersectCount] = CalcBezierX(curves[curveIndex], t2);
		intersectCount += 1 * hasIntersection;
	}
	intersections[0] = 0.0;
	
	// sort intersections
	for(int i = 1; i < intersectCount; i++) {
		float value = intersections[i];
		int j;
		for(j = i - 1; j > 0 && value < intersections[j]; j--) {
			intersections[j + 1] = intersections[j];
		}
		intersections[j + 1] = value;
	}
	
	// fill in pixels right to left
	float pixelOn = 0;
	int localX = glyphSize - 1;
	for(int intersect = intersectCount - 1; intersect >= 0; intersect--) {
		int intersectX = round(intersections[intersect] * glyphSize);
		for(; localX >= intersectX; localX--) {
			float tester = (float)intersect / (intersectCount - 1);
			outAtlas[uint2(DTid.x + localX, DTid.y)] = float4(tester, tester, tester, 1);//float4(pixelOn, pixelOn, pixelOn, 1);
		}
		pixelOn = 1.0 - pixelOn;
	}
}