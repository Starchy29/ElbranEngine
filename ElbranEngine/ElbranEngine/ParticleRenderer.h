#pragma once
#include "IRenderer.h"
#include "GeometryShader.h"

class ComputeShader;
class DXCore;

// must match HLSL struct
struct Particle {
    Vector2 worldPosition;
    Vector2 velocity;
    float timeLeft;
    float rotation;
    float width;
    int animationFrame;
};

class ParticleRenderer :
    public IRenderer
{
public:
    float lifespan;
    float spawnRadius;

    ParticleRenderer(unsigned int maxParticles);

    virtual void Update(float deltaTime) override;
    virtual void Draw(Camera* camera, const Transform& transform) override;
    virtual IBehavior* Clone() override;

    void Spawn(int amount, float duration = 0.f);
    void SetSpawnRate(float perSec);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> dxDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext;

    std::shared_ptr<GeometryShader> geometryShader;
    std::shared_ptr<ComputeShader> spawnShader;
    std::shared_ptr<ComputeShader> moveShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> computeParticleBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> computeParticleView;
    ArrayBuffer initalPosBuffer;

    unsigned int maxParticles;
    unsigned int particleCount;
    unsigned int lastIndex;

    float timer;
    int spawnsLeft;
    float spawnInterval;

    void Emit(int newParticles);
};

