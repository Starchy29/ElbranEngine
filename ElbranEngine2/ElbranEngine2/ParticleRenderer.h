#pragma once
#include "IRenderer.h"
#include "Common.h"
#include "Buffers.h"

// matches struct in ParticleSpawnCS.hlsl
struct ParticleSpawnState {
	Vector2 position;
	Vector2 velocity;
	float rotation;
};

class ParticleRenderer : public IRenderer {
public:
	EditBuffer particleBuffer;
	
	SpriteSheet sprites;
	float animationFPS;
	float lifespan;
	float startWidth;
	bool scaleWithParent;

	Color tint;
	bool applyLights;
	bool blendAdditive;

	ParticleRenderer() {}
	ParticleRenderer(uint16_t maxParticles, Texture2D sprite);
	ParticleRenderer(uint16_t maxParticles, SpriteSheet animation, float animationFPS);
	void Release();

	void Draw() override;
	void Emit(uint16_t numParticles, const ArrayBuffer* initialData);

	uint16_t GetMaxParticles() const;

private:
	uint16_t maxParticles;
	uint16_t lastParticle;
};

