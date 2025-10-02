#pragma once
#include "Buffers.h"
#include "Shaders.h"
#include "AudioData.h"
#include "Font.h"
#include <string>

class GraphicsAPI;
class SoundMixer;

class AssetContainer
{
public:
	Sampler defaultSampler;
	Mesh unitSquare;
	Mesh unitTriangle;

	VertexShader fullscreenVS;
	VertexShader cameraVS;
	VertexShader particlePassPS;

	GeometryShader particleQuadGS;

	PixelShader solidColorPS;
	PixelShader texturePS;
	PixelShader circleFillPS;
	PixelShader textRasterizePS;

	PixelShader conSatValPP;
	PixelShader blurPP;
	PixelShader bloomFilterPP;
	PixelShader screenSumPP;

	ComputeShader brightnessSumCS;
	ComputeShader particleSpawnCS;
	ComputeShader particleMoveCS;

	Texture2D testSprite;
	Font arial;

	AssetContainer() {}
	void Initialize(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio);
	void Release();
};

