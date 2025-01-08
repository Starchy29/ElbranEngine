#include "ShaderStructs.hlsli"

StructuredBuffer<Particle> particles : register(t0);

Particle main(uint index : SV_VERTEXID) {
	return particles[index];
}