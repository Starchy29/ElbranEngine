#include "ShaderStructs.hlsli"

cbuffer Scene : register(b0) {
	matrix viewProjection;
}

cbuffer Object : register(b1) {
	matrix worldTransform;
	bool flipX;
	bool flipY;
}

VertexToPixel main(Vertex input) {
	VertexToPixel output;

	float4 fullPosition = float4(input.position, 0, 1); // w must be 1 for translation to work
	fullPosition = mul(worldTransform, fullPosition);
	output.worldPosition = fullPosition.xy;
	output.screenPosition = mul(viewProjection, fullPosition);
	output.color = float4(1, 1, 1, 1);

	output.uv = input.uv;
	if(flipX) {
		output.uv.x = 1 - output.uv.x;
	}
	if(flipY) {
		output.uv.y = 1 - output.uv.y;
	}
	
	return output;
}