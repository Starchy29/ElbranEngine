#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float4 tint;
	float2 stretchFactor;
	float2 startUV;
	float2 endUV;
}

Texture2D Image : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	input.uv *= stretchFactor;
	float2 repeatStart = startUV * stretchFactor;
	float repeatEnd = endUV * stretchFactor;
	
	if(input.uv.x > repeatEnd.x) {
			
	}
	
	float4 pixel = Image.Sample(Sampler, input.uv);
    float4 result = pixel * tint;
	clip(result.a - 0.01);
	return result;
}