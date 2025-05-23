#include "ShaderStructs.hlsli"

float threshold : register(b0);
Texture2D Screen : register(t0);

float4 main(VertexToPixel input) : SV_TARGET {
    float3 startColor = Screen.Load(int3(input.screenPosition.xy, 0)).rgb;
    float luminance = dot(startColor, float3(0.21f, 0.72f, 0.07f));
    return float4(saturate(ceil(luminance - threshold + 1.0e-10)) * startColor, 1); // add a tiny bit to threshold to make it <=
}