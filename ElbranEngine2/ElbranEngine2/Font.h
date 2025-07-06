#pragma once
#include "Buffers.h"
#include "Math.h"
#include <string>
#include <unordered_map>

class AssetContainer;

struct Font {
	std::unordered_map<char, int> charToGlyphIndex;
	ArrayBuffer glyphCurves;
	ArrayBuffer firstCurveIndices;
	Vector2* glyphDimensions;
	float* glyphBaselines; // 0-1, where 0 is the bottom and 1 is the top

	void Release();

	static Font Load(std::wstring file);
};