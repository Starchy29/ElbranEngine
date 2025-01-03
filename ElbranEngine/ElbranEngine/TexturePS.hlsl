#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"

cbuffer Constants : register(b0) {
	float4 tint;
	float2 stretchFactor;
	bool lit;
}

Texture2D Image : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	input.uv *= stretchFactor;
	float4 pixel = Image.Sample(Sampler, input.uv) * tint;
	clip(pixel.a - 0.01);
	if(lit) {
		pixel = ApplyLights(pixel, input.worldPosition);
	}
	return pixel;
	pixel = float4(0, 0, 0, 0);
}