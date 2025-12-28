#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"

cbuffer Constants : register(b0) {
	float4 tint;
	bool lit;
	uint textureIndex;
}

Texture2DArray Image : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET {
	 // can pass in texture index through vertices or constant buffer
	float4 pixel = Image.Sample(Sampler, float3(input.uv, textureIndex + input.textureIndex)) * tint * input.color;
	clip(pixel.a - 1.0e-5);
	if(lit) pixel = ApplyLights(pixel, input.worldPosition);
	return pixel;
}