#pragma once
#include "Math.h"
#include "Common.h"

#define GPU_BOOL int32_t // bools are 4 bytes on the gpu

struct CameraVSConstants {
	Matrix worldTransform;
	Vector2 uvOffset;
	Vector2 uvScale;
};

struct TexturePSConstants {
	Color tint;
	GPU_BOOL lit;
};

struct ConSatValPPConstants {
	float contrast;
	float saturation;
	float brightness;
	float averageBrightness;
};

struct BlurPPConstants {
	UInt2 viewMin;
	UInt2 viewMax;
	uint32_t blurRadius;
	GPU_BOOL horizontal;
};

struct ParticleQuadGSConstants {
	float z;
	float spriteAspectRatio;
	int32_t animationFrames;
	float animationFPS;
	int32_t atlasRows;
	int32_t atlasCols;
};

struct ParticleMoveCSConstants {
	uint32_t maxParticles;
	float deltaTime;
	float growthRate;
	float spinRate;
	float fadeDuration;
};

struct ParticleSpawnCSConstants {
	uint32_t spawnCount;
	uint32_t lastParticle;
	uint32_t maxParticles;
	float lifespan;
	float width;
};

struct GlyphAtlasDrawCSConstants {
	uint32_t numGlyphs;
	uint32_t glyphSize; // pixel width of square for each glyph
	uint32_t rowsCols; // rows and cols are equal
};