#pragma once
#include "Buffers.h"
#include <string>
#include <unordered_map>

class AssetContainer;

struct Font {
	ComputeTexture glyphAtlas;
	std::unordered_map<char, int> charToGlyphIndex;

	void Release();

	static Font Load(std::wstring file, const AssetContainer* assets);
};