#include "ColorConversion.hlsli"

Texture2D Image : register(t0);
int2 screenDims : register(b0);
RWBuffer<uint> totalBrightnes : register(u0);

struct TestStruct {
	float decimal;
	int number;
};

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if(DTid.x >= screenDims.x || DTid.y >= screenDims.y) return;
	InterlockedAdd(totalBrightnes[0], (uint)(toHSV(Image.Load(DTid).rgb).z * 100));
}