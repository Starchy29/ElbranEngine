#ifndef __STRUCTS_INCLUDES__
#define __STRUCTS_INCLUDES__

struct VertexShaderInput {
	float2 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VertexToPixel {
	float4 screenPosition : SV_POSITION;
	float2 uv : TEXCOORD;
	float2 worldPosition : POSITION;
};
#endif