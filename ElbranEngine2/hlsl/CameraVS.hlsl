#include "ShaderStructs.hlsli"

cbuffer Object : register(b0) {
	matrix worldTransform;
	float2 uvOffset;
	float2 uvScale;
}

cbuffer Scene : register(b1) {
	matrix viewProjection;
}

VertexToPixel main(Vertex input) {
	VertexToPixel output;
	float4 fullPosition = float4(input.position, 0, 1);
	fullPosition = mul(worldTransform, fullPosition);
	output.worldPosition = fullPosition.xy;
	output.screenPosition = mul(viewProjection, fullPosition);
	output.color = float4(1, 1, 1, 1);
	output.uv = input.uv * uvScale + uvOffset;
	return output;
}