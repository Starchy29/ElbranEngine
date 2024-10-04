#ifndef __STRUCTS_INCLUDES__
#define __STRUCTS_INCLUDES__

struct VertexToPixel {
	float4 screenPosition : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};
#endif