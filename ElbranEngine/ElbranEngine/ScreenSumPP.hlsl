#include "ShaderStructs.hlsli"

Texture2D Screen1 : register(t0);
Texture2D Screen2 : register(t1);

float4 main(VertexToPixel input) : SV_TARGET
{
    return Screen1.Load(int3(input.screenPosition.xy, 0)) + Screen2.Load(int3(input.screenPosition.xy, 0));
}