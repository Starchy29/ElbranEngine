#pragma once
#include "Math.h"
#include "Common.h"

#define GPU_BOOL int // bools are 4 bytes on the gpu

struct CameraVSConstants {
	Matrix worldTransform;
	GPU_BOOL flipX;
	GPU_BOOL flipY;
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
	float frameDuration;
	int animationFrames;
	Vector2 spriteDims;
	int atlasCols;
};