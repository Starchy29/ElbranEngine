#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float z;
	float spriteAspectRatio;
	
	// for spriteSheet animation
	float animationFPS;
	uint atlasRows;
	uint atlasCols;
}

cbuffer Scene : register(b1) {
	matrix viewProjection;
}

[maxvertexcount(4)]
void main(point Particle input[1], inout TriangleStream<VertexToPixel> output) {
	if(input[0].timeLeft <= 0.f) {
		return;
	}
	
	float2 right = (input[0].width / 2.0f) * float2(cos(input[0].rotation), sin(input[0].rotation));
	float2 up = (input[0].width * spriteAspectRatio / 2.0f) * float2(-sin(input[0].rotation), cos(input[0].rotation));
	
	float4 corners[4] = {
		float4(input[0].worldPosition - right - up, z, 1),
		float4(input[0].worldPosition - right + up, z, 1),
		float4(input[0].worldPosition + right - up, z, 1),
		float4(input[0].worldPosition + right + up, z, 1)
	};
	
	float2 uvs[4] = {
		float2(0, 1),
		float2(0, 0),
		float2(1, 1),
		float2(1, 0)
	};
	
	float2 atlasPosition = float2(0, 0);
	uint animationFrames = atlasRows * atlasCols;
	int frame = animationFrames - 1 - (int) (input[0].timeLeft * animationFPS) % animationFrames;
	
	VertexToPixel vertex;
	
	[unroll]
	for(int i = 0; i < 4; i++) {
		vertex.screenPosition = mul(viewProjection, corners[i]);
		vertex.uv = uvs[i];
		vertex.worldPosition = corners[i].xy;
		vertex.color = float4(1, 1, 1, input[0].alpha);
		vertex.textureIndex = frame;
		
		output.Append(vertex);
	}
}