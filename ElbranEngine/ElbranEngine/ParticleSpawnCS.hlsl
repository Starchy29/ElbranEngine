#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	int spawnCount;
	uint lastParticle;
	uint maxParticles;
	float lifespan;
	float width;
	bool faceMoveDirection;
}

RWStructuredBuffer<Particle> particles : register(u0);
Buffer<float2> initialPositions : register(t0);
Buffer<float2> initialVelocities : register(t1);

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
	particles[spawnIndex].velocity = initialVelocities[DTid.x];
	particles[spawnIndex].width = width;
	particles[spawnIndex].timeLeft = lifespan;
	particles[spawnIndex].lifetime = 0;
	particles[spawnIndex].rotation = faceMoveDirection ? atan2(initialVelocities[DTid.x].y, initialVelocities[DTid.x].x) : 0;
	particles[spawnIndex].alpha = 1;
}