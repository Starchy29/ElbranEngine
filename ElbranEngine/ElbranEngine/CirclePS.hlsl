#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float4 color;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	float2 vectorFromMid = input.uv - float2(0.5, 0.5);
	float sqrDist = vectorFromMid.x * vectorFromMid.x + vectorFromMid.y * vectorFromMid.y;
    clip(0.5 * 0.5 - sqrDist); // if sqrDist > 0.5^2

	return color;
}