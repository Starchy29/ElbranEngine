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

	ParticleRenderer(unsigned int maxParticles, Texture2D sprite);
	ParticleRenderer(unsigned int maxParticles, SpriteSheet animation, float animationFPS);
	virtual ~ParticleRenderer();

	void Draw() override;
	void Emit(unsigned int numParticles, const ArrayBuffer* initialData);

	unsigned int GetMaxParticles() const;

private:
	unsigned int maxParticles;
	unsigned int lastParticle;
};

