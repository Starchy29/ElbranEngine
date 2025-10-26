#include "ShaderStructs.hlsli"

float4 color: register(b0);

float4 main(VertexToPixel input) : SV_TARGET {
	float2 fromMid = input.uv - float2(0.5, 0.5);
	float sqrDist = fromMid.x * fromMid.x + fromMid.y * fromMid.y;
    clip(0.5 * 0.5 - sqrDist); // if sqrDist > 0.5^2
	
	return color;
}