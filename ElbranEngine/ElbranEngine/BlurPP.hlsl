#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0)
{
    int2 viewMin;
    int2 viewMax;
    bool horizontal;
    int blurRadius;
}

Texture2D Screen : register(t0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float3 totalColor;
    float pixelCount;
    
	if(horizontal) {
        int startX = max(input.screenPosition.x - blurRadius, viewMin.x);
        int endX = min(input.screenPosition.x + blurRadius, viewMax.x);
        pixelCount = endX - startX + 1;
        
		for(int x = startX; x <= endX; x++) {
            totalColor += Screen.Load(int3(x, input.screenPosition.y, 0)).rgb;
        }
	} else {
        int startY = max(input.screenPosition.y - blurRadius, viewMin.y);
        int endY = min(input.screenPosition.y + blurRadius, viewMax.y);
        pixelCount = endY - startY + 1;
        
        for(int y = startY; y <= endY; y++) {
            totalColor += Screen.Load(int3(input.screenPosition.x, y, 0)).rgb;
        }
    }
    
    return float4(totalColor / pixelCount, 1);
}