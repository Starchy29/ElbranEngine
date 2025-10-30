#pragma once
#include "AppPointer.h"
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
	Texture2D testBMP;
	Font arial;

	AssetContainer() {}
	void Initialize(GraphicsAPI* graphics, SoundMixer* audio);
	void Release();

	Texture2D LoadBMP(std::wstring fileName);
	Texture2D LoadPNG(std::wstring fileName);
	void LoadWAV(std::wstring fileName);
};

