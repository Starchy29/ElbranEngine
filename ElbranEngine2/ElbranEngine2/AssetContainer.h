#pragma once
#include "Buffers.h"
#include "Shaders.h"
#include <string>

class GraphicsAPI;

class AssetContainer
{
public:
	Sampler defaultSampler;
	Mesh unitSquare;

	VertexShader fullscreenVS;
	VertexShader cameraVS;

	PixelShader solidColorPS;
	PixelShader texturePS;

	PixelShader conSatValPP;
	PixelShader blurPP;
	PixelShader bloomFilterPP;
	PixelShader screenSumPP;

	ComputeShader brightnessSumCS;

	Texture2D testSprite;
	Texture2D apple;

	AssetContainer(std::wstring filePath, GraphicsAPI* graphics);
	~AssetContainer();
};

