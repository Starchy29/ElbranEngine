#pragma once
#include "Renderer.h"

class GraphicsAPI;
class AssetContainer;
class MemoryArena;

class ParticleBehavior
{
public:
    enum class MoveStyle {
        None,
        Forward,
        Directional,
        Outward,
        Random
    };

    Renderer* particleRenderer;

    float speed;
    MoveStyle moveStyle;
    Vector2 moveDirection; // only used when moveStyle is set to Directional
    float totalGrowth;
    float spinRate;
    float fadeOutDuration;
    bool faceMoveDirection;
    bool spawnCircular;

    float lifespan;
    float startWidth;

    ParticleBehavior() = default;
    void Initialize(GraphicsAPI*, Renderer* particleRenderer);
    void Release(const GraphicsAPI*);

    void Update(GraphicsAPI*, const AssetContainer*, MemoryArena*, float deltaTime);

    void Emit(GraphicsAPI*, const AssetContainer*, MemoryArena*, uint16_t numParticles, float duration = 0.f);
    void SetSpawnRate(float perSec, uint16_t groupSize = 1);

private:
    ArrayBuffer spawnData;

    float timer;
    float spawnInterval;
    uint16_t spawnsPerInterval;
    int16_t spawnsLeft;

    uint16_t lastParticle;

    float CalcParentScaler();
};

