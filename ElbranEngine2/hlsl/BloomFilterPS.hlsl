#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
    float threshold;
    float sensitivity;
}
Texture2D Screen : register(t0);

float4 main(VertexToPixel input) : SV_TARGET {
    float3 startColor = Screen.Load(int3(input.screenPosition.xy, 0)).rgb;
    float luminance = dot(startColor, float3(0.21f, 0.72f, 0.07f));
    if(sensitivity <= 0) return float4(saturate(ceil(luminance - threshold)) * startColor, 1);
    return float4(saturate((luminance - threshold + sensitivity) / sensitivity) * startColor, 1);
}