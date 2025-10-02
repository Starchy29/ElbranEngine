#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	uint spawnCount;
	uint lastParticle;
	uint maxParticles;
	float lifespan;
	float width;
}

struct StartState {
	float2 position;
	float2 velocity;
	float rotation;
};

StructuredBuffer<StartState> initalStates : register(t0);
RWStructuredBuffer<Particle> particles : register(u0);

[numthreads(4, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	if(DTid.x >= spawnCount) {
		return;	
	}
	
	uint spawnIndex = (lastParticle + 1 + DTid.x) % maxParticles;	
	StartState startState = initalStates[DTid.x];
	
	particles[spawnIndex].worldPosition = startState.position;
	particles[spawnIndex].velocity = startState.velocity;
	particles[spawnIndex].width = width;
	particles[spawnIndex].timeLeft = lifespan;
	particles[spawnIndex].rotation = startState.rotation;
	particles[spawnIndex].alpha = 1;
}