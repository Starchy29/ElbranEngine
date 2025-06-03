#pragma once
#include "Buffers.h"
#include <string>
#include <unordered_map>

struct Font {
	ComputeTexture glyphAtlas;
	std::unordered_map<char, int> charToGlyphIndex;

	void Release();

	static Font Load(std::wstring file);
};