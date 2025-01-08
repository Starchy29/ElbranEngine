#pragma once
#include "IRenderer.h"
#include "GeometryShader.h"
#include "Color.h"

class ComputeShader;
class DXCore;
class Sprite;
class SpriteAtlas;

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
    // sprite settings
    Color tint;
    bool applyLights;

    // spawn settings
    float lifespan;
    float spawnRadius;

    // movement settings
    float width;

    ParticleRenderer(unsigned int maxParticles, float lifespan, std::shared_ptr<Sprite> sprite);
    ParticleRenderer(unsigned int maxParticles, float lifespan, std::shared_ptr<SpriteAtlas> animation);

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

    Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> readWriteView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> readOnlyView;
    ArrayBuffer initalPosBuffer;

    unsigned int maxParticles;
    unsigned int lastIndex;

    float timer;
    int spawnsLeft;
    float spawnInterval;

    bool animated; // if true, the below sprite stores a SpriteAtlas
    std::shared_ptr<Sprite> sprite;

    ParticleRenderer(unsigned int maxParticles, float lifespan);

    void Emit(int newParticles);
};

