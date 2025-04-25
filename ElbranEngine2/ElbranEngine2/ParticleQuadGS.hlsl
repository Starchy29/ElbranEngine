#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	float z;
	float spriteAspectRatio;
	
	// for spriteSheet animation
	float frameDuration;
	int animationFrames;
	float2 spriteDims;
	int atlasCols;
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
	
	float4 test = 0;
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
	if(animationFrames > 1 && frameDuration > 0) {
		int frame = (int)(input[0].lifetime / frameDuration) % animationFrames;
		atlasPosition = float2(frame % atlasCols, frame / atlasCols); // (col, row) to match with (x, y)
	}
	
	VertexToPixel vertex;
	
	[unroll]
	for(int i = 0; i < 4; i++) {
		vertex.screenPosition = mul(viewProjection, corners[i]);
		vertex.uv = spriteDims * (atlasPosition + uvs[i]);
		vertex.worldPosition = corners[i].xy;
		vertex.color = float4(1, 1, 1, input[0].alpha);
		output.Append(vertex);
	}
}