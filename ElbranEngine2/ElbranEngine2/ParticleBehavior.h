#pragma once
#include "IBehavior.h"
#include "ParticleRenderer.h"

class ParticleBehavior :
    public IBehavior
{
public:
    enum class MoveStyle {
        None,
        Forward,
        Directional,
        Outward,
        Random
    };

    ParticleRenderer* renderer;

    float speed;
    MoveStyle moveStyle;
    Vector2 moveDirection; // only used when moveStyle is set to Directional
    float totalGrowth;
    float spinRate;
    float fadeOutDuration;
    bool faceMoveDirection;
    bool spawnCircular;

    ParticleBehavior() {}
    void Initialize(ParticleRenderer* renderer);
    void Release();

    void Update(float deltaTime);

    void Spawn(uint16_t amount, float duration = 0.f);
    void SetSpawnRate(float perSec, uint16_t groupSize = 1);

private:
    ArrayBuffer spawnData;

    float timer;
    float spawnInterval;
    uint16_t spawnsPerInterval;
    int16_t spawnsLeft;

    float CalcParentScaler();
};

