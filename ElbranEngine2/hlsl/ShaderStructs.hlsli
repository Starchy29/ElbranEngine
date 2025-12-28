#ifndef __STRUCTS_INCLUDES__
#define __STRUCTS_INCLUDES__

struct Vertex {
	float2 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VertexToPixel {
	float4 screenPosition : SV_POSITION;
	float2 uv : TEXCOORD;
	float2 worldPosition : POSITION;
	float4 color : COLOR;
	uint textureIndex : INDEX;
};

struct Particle {
	float2 worldPosition : POSITION;
	float2 velocity : POSITION2;
	float timeLeft : HELPER0;
	float rotation : HELPER1;
	float width : HELPER2;
	float alpha : HELPER3;
};
#endif