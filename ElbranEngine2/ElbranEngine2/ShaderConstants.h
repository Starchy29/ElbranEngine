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