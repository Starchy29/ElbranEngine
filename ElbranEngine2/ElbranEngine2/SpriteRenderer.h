#pragma once
#include "GraphicsAPI.h"
#include "Common.h"
#include "Transform.h"

class SpriteRenderer
{
public:
	Transform* transform;
	const Matrix* worldMatrix;

	Texture2D* sprite;
	Color tint;
	bool lit;
	bool flipX;
	bool flipY;

	SpriteRenderer(Texture2D* sprite);
	
	void Draw();
};

