#include "ParticleBehavior.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"
#include "Random.h"

#define CONTINUOUS_SPAWN -1

struct ParticleSpawnState {
    Vector2 position;
    Vector2 velocity;
    float rotation;
};

void ParticleBehavior::Initialize(Renderer* particleRenderer) {
    ASSERT(particleRenderer && particleRenderer->type == Renderer::Type::Particles);
    this->particleRenderer = particleRenderer;
    moveStyle = MoveStyle::None;
    moveDirection = Vector2::Zero;
    speed = 0.f;
    totalGrowth = 0.f;
    spinRate = 0.f;
    fadeOutDuration = 0.f;
    faceMoveDirection = false;
    spawnCircular = false;
    timer = 0.f;
    spawnInterval = 0.f;
    spawnsPerInterval = 0u;
    spawnsLeft = 0;

    lifespan = 0.f;
    startWidth = 1.0f;
    lastParticle = 0;

    spawnData = app->graphics.CreateArrayBuffer(ShaderDataType::Structured, particleRenderer->particleData.maxParticles, sizeof(ParticleSpawnState));
}

void ParticleBehavior::Release() {
    app->graphics.ReleaseArrayBuffer(&spawnData);
}

void ParticleBehavior::Update(float deltaTime) {
    // updated particles
    GraphicsAPI* graphics = &app->graphics;

	ParticleMoveCSConstants csInput = {};
	csInput.deltaTime = deltaTime;
	csInput.maxParticles = particleRenderer->particleData.maxParticles;
    csInput.fadeDuration = fadeOutDuration;
    csInput.spinRate = spinRate;
    csInput.growthRate = totalGrowth / lifespan;
    if(particleRenderer->particleData.scaleWithParent) {
		csInput.growthRate *= CalcParentScaler();
    }

    ComputeShader* moveShader = &app->assets.particleMoveCS;
    graphics->WriteBuffer(&csInput, sizeof(ParticleMoveCSConstants), moveShader->constants.data);
    graphics->SetConstants(ShaderStage::Compute, &moveShader->constants, 0);
    graphics->SetEditBuffer(&particleRenderer->particleData.particleBuffer, 0);
    graphics->RunComputeShader(moveShader, particleRenderer->particleData.maxParticles, 1);
    graphics->SetEditBuffer(nullptr, 0); // unbind particles

    // update spawning
    if(spawnsLeft > 0 || spawnsLeft == CONTINUOUS_SPAWN) {
        timer -= deltaTime;
        if(timer <= 0.f) {
            timer = spawnInterval;
            if(spawnsLeft != CONTINUOUS_SPAWN) {
                spawnsLeft--;
            }
            Emit(spawnsPerInterval);
        }
    }
}

void ParticleBehavior::Emit(uint16_t numParticles, float duration) {
    if(duration > 0.f) {
        spawnsLeft += numParticles;
        spawnInterval = duration / numParticles;
        timer = spawnInterval;
        spawnsPerInterval = 1;
        return;
    }

    GraphicsAPI* graphics = &app->graphics;
    
    // set up initial states
    ParticleSpawnState* spawnStates = (ParticleSpawnState*)app->frameBuffer.Reserve(numParticles * sizeof(ParticleSpawnState), true);

    float trueSpeed = speed;
    float parentScalar = CalcParentScaler();
    if(particleRenderer->particleData.scaleWithParent) {
        trueSpeed *= parentScalar;
    }

    for(uint16_t i = 0; i < numParticles; i++) {
        spawnStates[i].position = *(particleRenderer->worldMatrix) * (spawnCircular ? app->rng.GenerateInCircle() * 0.5f : Vector2(app->rng.GenerateFloat(-0.5f, 0.5f), app->rng.GenerateFloat(-0.5f, 0.5f)));
    }
        
    float rotation;
    Vector2 center;
    Vector2 forward;
    switch(moveStyle) {
    case MoveStyle::Forward:
        center = *(particleRenderer->worldMatrix) * Vector2::Zero;
        forward = *(particleRenderer->worldMatrix) * Vector2::Right;
        forward = (forward - center).SetLength(trueSpeed);
        rotation = faceMoveDirection ? forward.Angle() : 0.f;
        for(uint16_t i = 0; i < numParticles; i++) {
            spawnStates[i].velocity = forward;
            spawnStates[i].rotation = rotation;
        }
        break;
    case MoveStyle::Directional:
        forward = moveDirection.SetLength(trueSpeed);
        rotation = faceMoveDirection ? moveDirection.Angle() : 0;
        for(uint16_t i = 0; i < numParticles; i++) {
            spawnStates[i].velocity = forward;
            spawnStates[i].rotation = rotation;
        }
        break;
    case MoveStyle::Outward:
        center = *(particleRenderer->worldMatrix) * Vector2::Zero;
        for(uint16_t i = 0; i < numParticles; i++) {
            spawnStates[i].velocity = (spawnStates[i].position - center).SetLength(trueSpeed);
            spawnStates[i].rotation = faceMoveDirection ? spawnStates[i].velocity.Angle() : 0.f;
        }
        break;
    case MoveStyle::Random:
        for(uint16_t i = 0; i < numParticles; i++) {
            spawnStates[i].rotation = app->rng.GenerateFloat(0.f, 2.f * PI);
            spawnStates[i].velocity = trueSpeed * Vector2::FromAngle(spawnStates[i].rotation);
            if(!faceMoveDirection) {
                spawnStates[i].rotation = 0.f;
            }
        }
        break;
    }
    
    // send initial data to gpu and run compute shader to update the particle data
    graphics->WriteBuffer(spawnStates, sizeof(ParticleSpawnState) * numParticles, spawnData.buffer);

    ParticleSpawnCSConstants csInput = {};
    csInput.spawnCount = numParticles;
    csInput.lastParticle = lastParticle;
    csInput.maxParticles = particleRenderer->particleData.maxParticles;
    csInput.lifespan = lifespan;
    csInput.width = startWidth;
    if(particleRenderer->particleData.scaleWithParent) {
        csInput.width *= parentScalar;
    }

    ComputeShader* spawnShader = &app->assets.particleSpawnCS;
    graphics->SetArray(ShaderStage::Compute, &spawnData, 0);
    graphics->SetEditBuffer(&particleRenderer->particleData.particleBuffer, 0);
    graphics->WriteBuffer(&csInput, numParticles * sizeof(ParticleSpawnCSConstants), spawnShader->constants.data);
    graphics->SetConstants(ShaderStage::Compute, &spawnShader->constants, 0);
    graphics->RunComputeShader(spawnShader, numParticles, 1);
    graphics->SetEditBuffer(nullptr, 0); // unbind particles

    lastParticle += numParticles;
    lastParticle %= particleRenderer->particleData.maxParticles;
}

void ParticleBehavior::SetSpawnRate(float perSec, uint16_t groupSize) {
    if(perSec <= 0.f) {
        // stop spawning
        spawnsLeft = 0;
        return;
    }

    spawnInterval = 1.0f / perSec;
    spawnsPerInterval = groupSize;
    timer = spawnInterval;
    spawnsLeft = CONTINUOUS_SPAWN;
}

float ParticleBehavior::CalcParentScaler() {
    Transform* parent = particleRenderer->transform->parent;
	Vector2 scale = Vector2(1.f, 1.f);
	while(parent) {
		scale *= parent->scale;
		parent = parent->parent;
	}
	return (scale.x + scale.y) / 2.0f;
}
