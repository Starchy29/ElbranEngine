#include "ParticleRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

#define PARTICLE_BYTES 36 // based on struct in ShaderStructs.hlsli

ParticleRenderer::ParticleRenderer(unsigned int maxParticles) {
	transform = nullptr;
	worldMatrix = nullptr;

	this->maxParticles = maxParticles;
	particleBuffer = app->graphics->CreateEditBuffer(ShaderDataType::Structured, maxParticles, PARTICLE_BYTES);
}

ParticleRenderer::~ParticleRenderer() {
	particleBuffer.Release();
}

void ParticleRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;
	// additive blend

	// vertex shader
	// unbind vertex buffer?
	graphics->SetArray(ShaderStage::Vertex, &particleBuffer, 0);
	graphics->SetVertexShader(&app->assets->particlePassPS);

	// geometry shader
	// set triangle primitive
	float globalZ = 0.f;
	const Transform* trans = transform;
	while(trans) {
		// can sum up parent zs because there is no z scale or rotation
		globalZ += trans->zOrder;
		trans = trans->parent;
	}

	ParticleQuadGSConstants gsInput = {};
	gsInput.z = globalZ;
	if(animated) {
		gsInput.animationFrames = animation->spriteCount;
		//gsin
	}
	graphics->SetGeometryShader(&app->assets->particleQuadGS, &gsInput, sizeof(ParticleQuadGSConstants));

	// pixel shader

	// draw
	graphics->DrawVertices(maxParticles);

	// clean up
	// unbind geo shader
}
