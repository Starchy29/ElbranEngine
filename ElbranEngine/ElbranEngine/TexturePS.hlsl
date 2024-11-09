#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float4 color;
}

Texture2D Image : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 pixel = Image.Sample(Sampler, input.uv);
	float4 result = pixel * color;
	clip(result.a - 0.01);
	return result;
}