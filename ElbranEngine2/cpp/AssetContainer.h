#pragma once
#include "GraphicsData.h"
#include "AudioData.h"
#include <string>

class GraphicsAPI;

class AssetContainer {
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
	SpriteSheet testSheet;
	AudioSample testSound;

	AssetContainer() = default;
	void Initialize(GraphicsAPI*);
	void Release(GraphicsAPI* graphics);

	static void ReleaseFont(const GraphicsAPI*, Font* font);

	VertexShader LoadVertexShader(const GraphicsAPI*, std::wstring fileName) const;
	GeometryShader LoadGeometryShader(const GraphicsAPI*, std::wstring fileName) const;
	PixelShader LoadPixelShader(const GraphicsAPI*, std::wstring fileName) const;
	ComputeShader LoadComputeShader(const GraphicsAPI*, std::wstring fileName) const;

	Texture2D LoadBMP(const GraphicsAPI*, std::wstring fileName) const;
	Texture2D LoadPNG(const GraphicsAPI*, std::wstring fileName) const;
	Font LoadTTF(const GraphicsAPI*, std::wstring fileName) const;
	AudioSample LoadWAV(std::wstring fileName) const;
};