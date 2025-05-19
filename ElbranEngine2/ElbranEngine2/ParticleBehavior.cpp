#include "ParticleBehavior.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"
#include "Random.h"

#define CONTINUOUS_SPAWN -1

ParticleBehavior::ParticleBehavior(ParticleRenderer* renderer) {
    this->renderer = renderer;

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
    spawnsPerInterval = 0;
    spawnsLeft = 0;

    spawnData = app->graphics->CreateArrayBuffer(ShaderDataType::Structured, renderer->GetMaxParticles(), sizeof(ParticleSpawnState));
}

ParticleBehavior::~ParticleBehavior() {
    app->graphics->ReleaseArrayBuffer(&spawnData);
}

void ParticleBehavior::Update(float deltaTime) {
    // updated particles
    GraphicsAPI* graphics = app->graphics;
    unsigned int maxParticles = renderer->GetMaxParticles();

	ParticleMoveCSConstants csInput = {};
	csInput.deltaTime = deltaTime;
	csInput.maxParticles = maxParticles;
    csInput.fadeDuration = fadeOutDuration;
    csInput.spinRate = spinRate;
    csInput.growthRate = totalGrowth / renderer->lifespan;
    if(renderer->scaleWithParent) {
		csInput.growthRate *= CalcParentScaler();
    }

    ComputeShader* moveShader = &app->assets->particleMoveCS;
    graphics->WriteBuffer(&csInput, sizeof(ParticleMoveCSConstants), moveShader->constants.data);
    graphics->SetConstants(ShaderStage::Compute, &moveShader->constants, 0);
    graphics->SetEditBuffer(&renderer->particleBuffer, 0);
    graphics->RunComputeShader(moveShader, maxParticles, 1);
    graphics->SetEditBuffer(nullptr, 0); // unbind particles

    // update spawning
    if(spawnsLeft > 0 || spawnsLeft == CONTINUOUS_SPAWN) {
        timer -= deltaTime;
        if(timer <= 0.f) {
            timer += spawnInterval;
            if(spawnsLeft != CONTINUOUS_SPAWN) {
                spawnsLeft--;
            }
            Spawn(spawnsPerInterval);
        }
    }
}

void ParticleBehavior::Spawn(unsigned int amount, float duration) {
    if(duration > 0.f) {
        spawnsLeft += amount;
        spawnInterval = duration / amount;
        timer = spawnInterval;
        spawnsPerInterval = 1;
        return;
    }
    
    Random* rng = app->rng;
    ParticleSpawnState* spawnStates = new ParticleSpawnState[amount] {};

    float trueSpeed = speed;
    if(renderer->scaleWithParent) {
        trueSpeed *= CalcParentScaler();
    }

    for(unsigned int i = 0; i < amount; i++) {
        spawnStates[i].position = *(renderer->worldMatrix) * (spawnCircular ? rng->GenerateInCircle() * 0.5f : Vector2(rng->GenerateFloat(-0.5f, 0.5f), rng->GenerateFloat(-0.5f, 0.5f)));
    }
        
    float rotation;
    Vector2 center;
    Vector2 forward;
    switch(moveStyle) {
    case MoveStyle::Forward:
        center = *(renderer->worldMatrix) * Vector2::Zero;
        forward = *(renderer->worldMatrix) * Vector2::Right;
        forward = (forward - center).SetLength(trueSpeed);
        rotation = faceMoveDirection ? forward.Angle() : 0.f;
        for(unsigned int i = 0; i < amount; i++) {
            spawnStates[i].velocity = forward;
            spawnStates[i].rotation = rotation;
        }
        break;
    case MoveStyle::Directional:
        forward = moveDirection.SetLength(trueSpeed);
        rotation = faceMoveDirection ? moveDirection.Angle() : 0;
        for(unsigned int i = 0; i < amount; i++) {
            spawnStates[i].velocity = forward;
            spawnStates[i].rotation = rotation;
        }
        break;
    case MoveStyle::Outward:
        center = *(renderer->worldMatrix) * Vector2::Zero;
        for(unsigned int i = 0; i < amount; i++) {
            spawnStates[i].velocity = (spawnStates[i].position - center).SetLength(trueSpeed);
            spawnStates[i].rotation = faceMoveDirection ? spawnStates[i].velocity.Angle() : 0.f;
        }
        break;
    case MoveStyle::Random:
        for(unsigned int i = 0; i < amount; i++) {
            spawnStates[i].rotation = rng->GenerateFloat(0.f, 2.f * PI);
            spawnStates[i].velocity = trueSpeed * Vector2::FromAngle(spawnStates[i].rotation);
            if(!faceMoveDirection) {
                spawnStates[i].rotation = 0.f;
            }
        }
        break;
    }
    
    app->graphics->WriteBuffer(spawnStates, sizeof(ParticleSpawnState) * amount, spawnData.buffer);
    renderer->Emit(amount, &spawnData);
    delete[] spawnStates;
}

void ParticleBehavior::SetSpawnRate(float perSec, unsigned int groupSize) {
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
    Transform* parent = renderer->transform->parent;
	Vector2 scale = Vector2(1.f, 1.f);
	while(parent) {
		scale *= parent->scale;
		parent = parent->parent;
	}
	return (scale.x + scale.y) / 2.0f;
}
