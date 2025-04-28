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

    ParticleBehavior(ParticleRenderer* renderer);
    ~ParticleBehavior();

    void Update(float deltaTime);

    void Spawn(unsigned int amount, float duration = 0.f);
    void SetSpawnRate(float perSec, unsigned int groupSize = 1);

private:
    ArrayBuffer spawnData;

    float timer;
    float spawnInterval;
    unsigned int spawnsPerInterval;
    int spawnsLeft;

    float CalcParentScaler();
};

