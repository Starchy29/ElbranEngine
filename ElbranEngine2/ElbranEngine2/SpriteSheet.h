#pragma once
#include "Buffers.h"

struct SpriteSheet {
	Texture2D texture;
	int rows;
	int cols;
	int spriteCount;
	float spriteAspectRatio;
};