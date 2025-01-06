#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	int spawnCount;
	uint lastParticle;
	uint maxParticles;
	float lifespan;
}

RWStructuredBuffer<Particle> particles : register(u0);
Buffer<float2> initialPositions : register(t1);
// inital velocity

[numthreads(4, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	if(DTid.x >= spawnCount) {
		return;	
	}
	
	uint spawnIndex = lastParticle + 1 + DTid.x;
	if(spawnIndex >= maxParticles) {
		spawnIndex -= maxParticles;	
	}
	
	particles[spawnIndex].worldPosition = initialPositions[DTid.x];
	particles[spawnIndex].velocity = float2(0, 0);
	particles[spawnIndex].timeLeft = lifespan;
	particles[spawnIndex].rotation = 0;
	particles[spawnIndex].width = 1;
	particles[spawnIndex].animationFrame = 0;
}