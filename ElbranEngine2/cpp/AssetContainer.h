#pragma once
#include "GraphicsData.h"
#include "AudioData.h"

class GraphicsAPI;
class MemoryArena;

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
	ComputeShader particleClearCS;

	Texture2D testSprite;
	Texture2D testBMP;
	Font arial;
	SpriteSheet testSheet;
	AudioSample testSound;

	AssetContainer() = default;
	void Initialize(GraphicsAPI*, MemoryArena*);
	void Release(GraphicsAPI*);

	static void ReleaseFont(const GraphicsAPI*, Font*);

	VertexShader LoadVertexShader(const GraphicsAPI*, const char* fileName) const;
	GeometryShader LoadGeometryShader(const GraphicsAPI*, const char* fileName) const;
	PixelShader LoadPixelShader(const GraphicsAPI*, const char* fileName) const;
	ComputeShader LoadComputeShader(const GraphicsAPI*, const char* fileName) const;

	Texture2D LoadBMP(const GraphicsAPI*, const char* fileName) const;
	Texture2D LoadPNG(const GraphicsAPI*, const char* fileName) const;
	Font LoadTTF(const GraphicsAPI*, MemoryArena* arena, const char* fileName) const;
	AudioSample LoadWAV(const char* fileName) const;
};