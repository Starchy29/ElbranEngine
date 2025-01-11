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
	float4 color : COLOR;
};

struct Particle {
	float2 worldPosition : POSITION;
	float2 velocity : POSITION2;
	float timeLeft : HELPER0;
	float lifetime : HELPER1;
	float rotation : HELPER2;
	float width : HELPER3;
	float alpha : HELPER4;
};
#endif