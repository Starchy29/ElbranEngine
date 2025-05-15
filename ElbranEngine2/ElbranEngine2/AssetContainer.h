#pragma once
#include "Buffers.h"
#include "Shaders.h"
#include "AudioData.h"
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

	PixelShader conSatValPP;
	PixelShader blurPP;
	PixelShader bloomFilterPP;
	PixelShader screenSumPP;

	ComputeShader brightnessSumCS;
	ComputeShader particleSpawnCS;
	ComputeShader particleMoveCS;

	Texture2D testSprite;
	Texture2D apple;
	SpriteSheet testAtlas;

	SoundEffect testSound;
	AudioTrack testMusic;

	AssetContainer(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio);
	~AssetContainer();
};

