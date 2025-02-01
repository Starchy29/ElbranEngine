	#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	uint maxParticles;
	float deltaTime;
}

cbuffer ParticleType : register(b1) {
	float growthRate;
	float spinRate;
	float fadeDuration;
}

RWStructuredBuffer<Particle> particles : register(u0);

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	if(DTid.x >= maxParticles) {
		return;
	}
	
	Particle particle = particles[DTid.x];
	if(particle.timeLeft <= 0) {
		return;	
	}
	
	particle.timeLeft -= deltaTime;
	particle.lifetime += deltaTime;
	
	particle.worldPosition += particle.velocity * deltaTime;
	particle.width = max(0, particle.width + growthRate * deltaTime);
	particle.rotation += spinRate * deltaTime;
	particle.alpha = fadeDuration > 0 ? saturate(particle.timeLeft / fadeDuration) : 1;
	
	particles[DTid.x] = particle;
}