#include "SpriteAtlas.h"

SpriteAtlas::SpriteAtlas(std::wstring fileName, int rows, int cols, int numSprites)
	: Sprite(fileName)
{
	this->rows = rows;
	this->cols = cols;
	this->numSprites = numSprites;

	aspectRatio = ((float)pixelWidth / cols) / ((float)pixelHeight / rows);
}

int SpriteAtlas::NumRows() const {
	return rows;
}

int SpriteAtlas::NumCols() const {
	return cols;
}

int SpriteAtlas::SpriteCount() const {
	return numSprites;
}
