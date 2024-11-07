#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	matrix worldViewProj;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	output.screenPosition = mul(worldViewProj, float4(input.position, 0, 1));
	output.uv = input.uv;

	return output;
}