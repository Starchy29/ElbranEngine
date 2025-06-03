#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"

cbuffer Constants : register(b0) {
	float4 tint;
	bool lit;
}

Texture2D Image : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET {
	float4 pixel = Image.Sample(Sampler, input.uv) * tint * input.color;
	clip(pixel.a - 1.0e-5);
	if(lit) {
		pixel = ApplyLights(pixel, input.worldPosition);
	}
	return pixel;
}