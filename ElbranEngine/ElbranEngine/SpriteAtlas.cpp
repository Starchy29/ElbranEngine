#include "SpriteAtlas.h"

SpriteAtlas::SpriteAtlas(std::wstring fileName, int rows, int cols)
	: Sprite(fileName)
{
	this->rows = rows;
	this->cols = cols;

	aspectRatio = ((float)pixelWidth / cols) / ((float)pixelHeight / rows);
}

int SpriteAtlas::NumRows() const {
	return rows;
}

int SpriteAtlas::NumCols() const {
	return cols;
}
