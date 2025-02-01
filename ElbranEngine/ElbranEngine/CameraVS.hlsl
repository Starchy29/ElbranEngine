#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	matrix worldViewProj;
	matrix worldTransform;
	bool flipX;
	bool flipY;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	float4 fullPosition = float4(input.position, 0, 1);
	output.screenPosition = mul(worldViewProj, fullPosition);
	output.worldPosition = mul(worldTransform, fullPosition).xy;
	output.color = 1;

	output.uv = input.uv;
	if(flipX) {
		output.uv.x = 1 - output.uv.x;
	}
	if(flipY) {
		output.uv.y = 1 - output.uv.y;
	}
	
	return output;
}