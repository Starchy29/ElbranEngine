#pragma once
#include "IRenderer.h"
#include "Common.h"
#include "Buffers.h"

class PatternRenderer :
    public IRenderer
{
public:
	const Texture2D* sprite;
	Vector2 origin;
	Vector2 blockSize;

	Color tint;
	bool lit;
	bool flipX;
	bool flipY;

	PatternRenderer(const Texture2D* sprite);

	void Draw() override;
};

