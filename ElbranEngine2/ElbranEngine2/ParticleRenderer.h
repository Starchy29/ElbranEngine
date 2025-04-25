#pragma once
#include "IRenderer.h"
#include "SpriteSheet.h"

class ParticleRenderer : public IRenderer {
public:
	EditBuffer particleBuffer;
	
	union {
		Texture2D* sprite;
		SpriteSheet* animation;
	};
	bool animated;

	ParticleRenderer(unsigned int maxParticles);
	virtual ~ParticleRenderer();

	void Draw() override;

private:
	unsigned int maxParticles;
};

