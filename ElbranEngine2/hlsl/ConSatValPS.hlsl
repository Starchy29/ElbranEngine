#include "ColorConversion.hlsli"
#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
    float contrast;
    float saturation;
    float brightness;
    float averageBrightness;
}

Texture2D Screen : register(t0);

float4 main(VertexToPixel input) : SV_TARGET {
	float3 startColor = Screen.Load(int3(input.screenPosition.xy, 0)).rgb;
    float3 hsv = toHSV(startColor);
    
    // adjust saturation
    if(saturation > 0) {
        hsv.y = lerp(hsv.y, 1, saturation);
    } else {
        hsv.y = lerp(hsv.y, 0, -saturation);
    }
    
    // adjust contrast
    if(contrast != 0) {
        float delta = hsv.z - averageBrightness;
        float multiplier = max(contrast, -1);
        if(contrast > 0) {
            multiplier *= 2.0;
        }
        delta *= (multiplier + 1.0); // range: 0 - 3
        hsv.z = saturate(averageBrightness + delta);
    }
    
    // adjust brightness
    float3 endColor = toRGB(hsv);
    if(brightness > 0) {
        endColor = lerp(endColor, float3(1, 1, 1), brightness);

    } else {
        endColor = lerp(endColor, float3(0, 0, 0), -brightness);
    }
    
    return float4(endColor, 1);
}