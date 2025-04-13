#pragma once
#include "GraphicsAPI.h"
#include "Common.h"

class SpriteRenderer
{
public:
	const Texture2D* sprite;
	Color tint;
	bool lit;
	bool flipX;
	bool flipY;

	SpriteRenderer(const Texture2D* sprite);
	
	void Draw();
};

