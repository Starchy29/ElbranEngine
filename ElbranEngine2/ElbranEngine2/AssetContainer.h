#pragma once
#include "Buffers.h"
#include "Shaders.h"

class GraphicsAPI;

class AssetContainer
{
public:
	Sampler defaultSampler;
	Mesh unitSquare;
	VertexShader fullscreenShader;

	AssetContainer(GraphicsAPI* graphics);
	~AssetContainer();
};

