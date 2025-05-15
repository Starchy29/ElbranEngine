#pragma once
#include "Math.h"
#include "Common.h"

#define GPU_BOOL int // bools are 4 bytes on the gpu

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
	int blurRadius;
	GPU_BOOL horizontal;
};

struct ParticleQuadGSConstants {
	float z;
	float spriteAspectRatio;
	int animationFrames;
	float animationFPS;
	int atlasRows;
	int atlasCols;
};

struct ParticleMoveCSConstants {
	unsigned int maxParticles;
	float deltaTime;
	float growthRate;
	float spinRate;
	float fadeDuration;
};

struct ParticleSpawnCSConstants {
	unsigned int spawnCount;
	unsigned int lastParticle;
	unsigned int maxParticles;
	float lifespan;
	float width;
};