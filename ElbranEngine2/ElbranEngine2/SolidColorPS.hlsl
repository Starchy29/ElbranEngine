#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float4 color;
}

float4 main(VertexToPixel input) : SV_TARGET {
	return color;
}