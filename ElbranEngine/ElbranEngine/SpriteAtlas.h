#pragma once
#include "Sprite.h"

class SpriteAtlas
	: public Sprite
{
public:
	SpriteAtlas(std::wstring fileName, int rows, int cols, int numSprites);

	int NumRows() const;
	int NumCols() const;
	int SpriteCount() const;

private:
	int rows;
	int cols;
	int numSprites;
};

