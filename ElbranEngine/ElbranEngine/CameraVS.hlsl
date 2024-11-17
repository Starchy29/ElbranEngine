#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	matrix worldViewProj;
	bool flipX;
	bool flipY;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	output.screenPosition = mul(worldViewProj, float4(input.position, 0, 1));

	output.uv = input.uv;
	if(flipX) {
		output.uv.x = 1 - output.uv.x;
	}
	if(flipY) {
		output.uv.y = 1 - output.uv.y;
	}

	return output;
}