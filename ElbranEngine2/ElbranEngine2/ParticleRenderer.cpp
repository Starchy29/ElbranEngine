#include "ParticleRenderer.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

#define PARTICLE_BYTES 32 // based on struct in ShaderStructs.hlsli

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, Texture2D sprite) 
	: ParticleRenderer(maxParticles, SpriteSheet { sprite, 1, 1, 1 }, 0.f)
{ }

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, SpriteSheet animation, float animationFPS) {
	transform = nullptr;
	worldMatrix = nullptr;

	sprites = animation;
	this->animationFPS = animationFPS;

	this->maxParticles = maxParticles;
	lastParticle = -1;

	tint = Color::White;
	applyLights = false;
	blendAdditive = false;
	scaleWithParent = true;
	startWidth = 1.0f;
	lifespan = 1.0f;

	particleBuffer = app->graphics->CreateEditBuffer(ShaderDataType::Structured, maxParticles, PARTICLE_BYTES);
}

ParticleRenderer::~ParticleRenderer() {
	app->graphics->ReleaseEditBuffer(&particleBuffer);
}

void ParticleRenderer::Draw() {
	GraphicsAPI* graphics = app->graphics;

	// vertex shader
	graphics->ClearMesh();
	graphics->SetArray(ShaderStage::Vertex, &particleBuffer, 0);
	graphics->SetVertexShader(&app->assets->particlePassPS);

	// geometry shader
	float globalZ = 0.f;
	const Transform* trans = transform;
	while(trans) {
		// can sum up parent zs because there is no z scale or rotation
		globalZ += trans->zOrder;
		trans = trans->parent;
	}

	ParticleQuadGSConstants gsInput = {};
	gsInput.z = globalZ;
	gsInput.spriteAspectRatio = sprites.SpriteAspectRatio();
	gsInput.animationFPS = animationFPS;
	gsInput.animationFrames = sprites.spriteCount;
	gsInput.atlasRows = sprites.rows;
	gsInput.atlasCols = sprites.cols;
	graphics->SetGeometryShader(&app->assets->particleQuadGS, &gsInput, sizeof(ParticleQuadGSConstants));

	// pixel shader
	TexturePSConstants psInput;
	psInput.lit = applyLights;
	psInput.tint = tint;
	graphics->SetTexture(ShaderStage::Pixel, &sprites.texture, 0);
	graphics->SetPixelShader(&app->assets->texturePS, &psInput, sizeof(psInput));

	// draw
	if(blendAdditive) {
		graphics->SetBlendMode(BlendState::Additive);
	}
	graphics->SetPrimitive(RenderPrimitive::Point);
	graphics->DrawVertices(maxParticles);

	// clean up
	if(blendAdditive) {
		graphics->SetBlendMode(BlendState::AlphaBlend);
	}
	graphics->SetPrimitive(RenderPrimitive::Triangle);
	graphics->SetGeometryShader(nullptr);
	graphics->SetArray(ShaderStage::Vertex, nullptr, 0); // unbind particles
}

void ParticleRenderer::Emit(unsigned int numParticles, const ArrayBuffer* initialData) {
	GraphicsAPI* graphics = app->graphics;
	ComputeShader* spawnShader = &app->assets->particleSpawnCS;

	ParticleSpawnCSConstants csInput = {};
	csInput.spawnCount = numParticles;
	csInput.lastParticle = lastParticle;
	csInput.maxParticles = maxParticles;
	csInput.lifespan = lifespan;
	csInput.width = startWidth;
	if(scaleWithParent) {
		Transform* parent = transform->parent;
		Vector2 scale = Vector2(1.f, 1.f);
		while(parent) {
			scale *= parent->scale;
			parent = parent->parent;
		}
		csInput.width *= (scale.x + scale.y) / 2.0f;
	}

	graphics->SetArray(ShaderStage::Compute, initialData, 0);
	graphics->SetEditBuffer(&particleBuffer, 0);
	graphics->WriteBuffer(&csInput, numParticles * sizeof(ParticleSpawnCSConstants), spawnShader->constants.data);
	graphics->SetConstants(ShaderStage::Compute, &spawnShader->constants, 0);
	graphics->RunComputeShader(spawnShader, numParticles, 1);
	graphics->SetEditBuffer(nullptr, 0); // unbind particles

	lastParticle += numParticles;
	lastParticle %= maxParticles;
}

unsigned int ParticleRenderer::GetMaxParticles() const {
	return maxParticles;
}
