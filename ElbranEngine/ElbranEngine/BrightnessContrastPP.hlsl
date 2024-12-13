#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0)
{
    float brightness;
    float contrast;
}

Texture2D Screen : register(t0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 startColor = Screen.Load(int3(input.screenPosition.xy, 0));
    startColor.r += 0.3f;
    return startColor;
}