#pragma once
#include <string>
#include "Common.h"

struct Texture
{
	Texture(std::wstring fileName);
	Texture(int width, int height);

	Int2 DetermineDimensions(); // move to helper in DirectXAPI?
};

