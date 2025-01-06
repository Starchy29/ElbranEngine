#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	int particleCount;
	float deltaTime;
}

cbuffer ParticleType : register(b1) {
	float2 acceleration;
}

RWStructuredBuffer<Particle> particles : register(u0);

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	if(DTid.x >= particleCount) {
		return;
	}
	
	Particle particle = particles[DTid.x];
	if(particle.timeLeft <= 0) {
		return;	
	}
	
	particle.timeLeft -= deltaTime;
	
	particle.velocity += acceleration * deltaTime;
	particle.worldPosition += particle.velocity * deltaTime;
	
	particles[DTid.x] = particle;
}