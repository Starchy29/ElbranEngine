#include "ShaderStructs.hlsli"
#include "ColorConversion.hlsli"

cbuffer Constants : register(b0)
{
    float contrast;
    float saturation;
    float brightness;
}

Texture2D Screen : register(t0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float3 startColor = Screen.Load(int3(input.screenPosition.xy, 0)).rgb;
    float3 hsv = toHSV(startColor);
    
    // adjust saturation
    if(saturation > 0) {
        hsv.y = lerp(hsv.y, 1, saturation);

    } else {
        hsv.y = lerp(hsv.y, 0, -saturation);
    }
    
    float3 endColor = toRGB(hsv);
    
    // adjust brightness
    if(brightness > 0) {
        endColor = lerp(endColor, float3(1, 1, 1), brightness);

    } else {
        endColor = lerp(endColor, float3(0, 0, 0), -brightness);
    }
    
    // adjust contrast
    if(contrast > 0) {
        endColor = lerp(endColor, round(endColor), contrast);
    } else {
        endColor = lerp(endColor, float3(0.5, 0.5, 0.5), -contrast);
    }
    
    return float4(endColor, 1);

}