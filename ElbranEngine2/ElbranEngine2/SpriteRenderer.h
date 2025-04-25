#pragma once
#include "Common.h"
#include "Buffers.h"
#include "IRenderer.h"

class SpriteRenderer : public IRenderer {
public:
	Texture2D* sprite;
	Color tint;
	bool lit;
	bool flipX;
	bool flipY;

	SpriteRenderer(Texture2D* sprite);
	
	void Draw() override;
};

