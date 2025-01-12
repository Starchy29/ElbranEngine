#pragma once
#include "IRenderer.h"
#include "GeometryShader.h"
#include "Color.h"

class ComputeShader;
class DXCore;
class Sprite;
class SpriteAtlas;

enum class ParticleMovement {
    None,
    Forward,
    Directional,
    Outward,
    Random
};

class ParticleRenderer :
    public IRenderer
{
public:
    Color tint;
    bool applyLights;

    float lifespan;
    float width;
    bool spawnCircular;
    bool scaleWithParent;

    float speed;
    ParticleMovement moveStyle;
    Vector2 moveDirection; // only used when moveStyle is set to Directional
    bool faceMoveDirection;

    float totalGrowth;
    float spinRate;
    float fadeDuration;
    float secondsPerFrame;

    ParticleRenderer(unsigned int maxParticles, float lifespan, bool blendAdditive, std::shared_ptr<Sprite> sprite);
    ParticleRenderer(unsigned int maxParticles, float lifespan, bool blendAdditive, std::shared_ptr<SpriteAtlas> animation);

    virtual void Update(float deltaTime) override;
    virtual void Draw(Camera* camera, const Transform& transform) override;
    virtual IBehavior* Clone() override;

    void Spawn(int amount, float duration = 0.f);
    void SetSpawnRate(float perSec, int groupSize = 1);
    bool BlendsAdditive() const;

protected:
    DXCore* directX;
    Microsoft::WRL::ComPtr<ID3D11Device> dxDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext;

    std::shared_ptr<GeometryShader> geometryShader;
    std::shared_ptr<ComputeShader> spawnShader;
    std::shared_ptr<ComputeShader> moveShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> readWriteView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> readOnlyView;
    ArrayBuffer initalPosBuffer;
    ArrayBuffer initialVelBuffer;

    unsigned int maxParticles;
    unsigned int lastIndex;

    float timer;
    int spawnsLeft;
    float spawnInterval;
    int spawnsPerInterval;

    bool blendAdditive;
    bool animated; // if true, the below sprite stores a SpriteAtlas
    std::shared_ptr<Sprite> sprite;

    ParticleRenderer(unsigned int maxParticles, float lifespan, bool blendAdditive);

    void Emit(int newParticles);
    virtual void SetSpawnData(int newParticles);
};

