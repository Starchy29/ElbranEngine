#include "ColorConversion.hlsli"

#define ARRAY_SIZE 4

Texture2D Image : register(t0);
uint2 screenDims : register(b0);
RWBuffer<uint> totalBrightness : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	if(DTid.x >= screenDims.x || DTid.y >= screenDims.y) return;
	InterlockedAdd(totalBrightness[DTid.x % ARRAY_SIZE], (uint)(toHSV(Image.Load(DTid).rgb).z * 100));
}