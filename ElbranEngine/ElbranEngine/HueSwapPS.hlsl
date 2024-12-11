#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
    float4 tint;
	float4 replacedColor;
    float4 replacementColor;
    float sensitivity;
}

Texture2D Image : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 color = Image.Sample(Sampler, input.uv);
    float3 testDiff = replacedColor.rgb - color.rgb;
    if(abs(testDiff.r) + abs(testDiff.g) + abs(testDiff.b) <= sensitivity) {
        color = float4(saturate(replacementColor.rgb + testDiff), color.a);
    }
	
	clip(color.a - 0.01);
    return color;
}