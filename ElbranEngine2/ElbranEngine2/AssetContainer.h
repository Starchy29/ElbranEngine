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

	PixelShader texturePS;

	Texture2D testSprite;

	AssetContainer(std::wstring filePath, GraphicsAPI* graphics);
	~AssetContainer();
};

