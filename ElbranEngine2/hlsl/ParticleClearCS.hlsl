#include "ShaderStructs.hlsli"

cbuffer Constants : register(b0) {
	uint maxParticles;
}

RWStructuredBuffer<Particle> particles : register(u0);

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	if(DTid.x >= maxParticles) return;
	particles[DTid.x].timeLeft = 0.0;
}