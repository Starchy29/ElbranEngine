#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	matrix worldTransform;
	matrix view;
	matrix projection;
}

struct VertexShaderInput {
	float2 position : POSITION;
	float2 uv : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix worldViewProj = mul(projection, mul(view, worldTransform));
	output.screenPosition = mul(worldViewProj, float4(input.position, 0, 1));
	output.uv = input.uv;

	return output;
}