#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
    uint2 viewMin;
    uint2 viewMax;
    int blurRadius;
    bool horizontal;
}

Texture2D Screen : register(t0);

float weightGuassian(float distance) {
    float sqrDist = distance * distance;
    float sigma = (2 * blurRadius + 1) / 6.0; // base sigma off of the pixel range
    float denominator = 2 * sigma * sigma;
    return exp(-sqrDist / denominator);
}

float4 main(VertexToPixel input) : SV_TARGET {
    float3 totalColor;
    float totalWeight;
    
	if(horizontal) {
        int startX = max(input.screenPosition.x - blurRadius, viewMin.x);
        int endX = min(input.screenPosition.x + blurRadius, viewMax.x);
        
		for(int x = startX; x <= endX; x++) {
            float weight = weightGuassian(abs(x - input.screenPosition.x));
            totalWeight += weight;
            totalColor += weight * Screen.Load(int3(x, input.screenPosition.y, 0)).rgb;
        }
	} else {
        int startY = max(input.screenPosition.y - blurRadius, viewMin.y);
        int endY = min(input.screenPosition.y + blurRadius, viewMax.y);
        
        for(int y = startY; y <= endY; y++) {
            float weight = weightGuassian(abs(y - input.screenPosition.y));
            totalWeight += weight;
            totalColor += weight * Screen.Load(int3(input.screenPosition.x, y, 0)).rgb;
        }
    }

    return float4(totalColor / totalWeight, 1);
}