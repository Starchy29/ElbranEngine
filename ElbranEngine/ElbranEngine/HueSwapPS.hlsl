#include "ShaderStructs.hlsli"
#include "ColorConversion.hlsli"
#include "Lighting.hlsli"

cbuffer Constants : register(b0) {
    float4 tint;
	float4 replacedColor;
    float4 replacementColor;
    float sensitivity;
    bool lit;
}

Texture2D Image : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color = Image.Sample(Sampler, input.uv);
    clip(color.a - 0.01);
    
    float3 sampleHSV = toHSV(color.rgb);
    float3 testHSV = toHSV(replacedColor.rgb);
    float3 replacementHSV = toHSV(replacementColor.rgb);
    
    if(testHSV.y <= 0) {
        // when replacing grayscale, check brightness instead of hue
        float brightnessDiff = sampleHSV.z - testHSV.z;
        if(sampleHSV.y <= 0 && brightnessDiff <= sensitivity) {
            replacementHSV.z = saturate(sampleHSV.z + brightnessDiff);
            color = float4(toRGB(replacementHSV), color.a);
        }
        
        if(lit) {
		    color = ApplyLights(color, input.worldPosition);
	    }
        return color;
    }
    
    float hueDiff = sampleHSV.x - testHSV.x;
    if(abs(hueDiff) <= sensitivity) {
        if(replacementHSV.y <= 0) {
            // when replacing with grayscale, set the saturation to 0
            sampleHSV.y = 0;
        } else {
            // change the hue and keep the sampled saturation and brightness
            sampleHSV.x = replacementHSV.x + hueDiff;
        }
        
        color = float4(toRGB(sampleHSV), color.a);
    }
	
    if(lit) {
		color = ApplyLights(color, input.worldPosition);
	}
    return color;
}