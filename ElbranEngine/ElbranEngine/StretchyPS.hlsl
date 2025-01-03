#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"

cbuffer Constants : register(b0) {
	float4 tint;
	float2 stretchFactor;
	float2 startUV;
	float2 endUV;
	bool lit;
}

Texture2D Image : register(t0);
SamplerState Sampler : register(s0); // must wrap UVs

float4 main(VertexToPixel input) : SV_TARGET
{
	// scale the UV by the stretch factor to represent how many tiles to draw
	// for 0 - startUV, draw the start section
	// for startUV - endSpot, repeat the middle section
	// for endSpot - stretchedFactor, draw the end section
	
	float2 endSpot = stretchFactor - (1.0 - endUV);
	float2 sampleUV = input.uv * stretchFactor;
	sampleUV = ( min(sampleUV, startUV) + max(0, sampleUV - startUV) % (endUV - startUV) ) * (sampleUV <= endSpot)
		+ (1.0 - stretchFactor + sampleUV) * (sampleUV > endSpot);

	float4 pixel = Image.SampleLevel(Sampler, sampleUV, 0) * tint;
	clip(pixel.a - 0.01);
	if(lit) {
		pixel = ApplyLights(pixel, input.worldPosition);
	}
	return pixel;
}