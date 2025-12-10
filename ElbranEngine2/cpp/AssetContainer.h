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

	static VertexShader LoadVertexShader(const GraphicsAPI*, const char* fileName);
	static GeometryShader LoadGeometryShader(const GraphicsAPI*, const char* fileName);
	static PixelShader LoadPixelShader(const GraphicsAPI*, const char* fileName) ;
	static ComputeShader LoadComputeShader(const GraphicsAPI*, const char* fileName);

	static Texture2D LoadBMP(const GraphicsAPI*, const char* fileName);
	static Texture2D LoadPNG(const GraphicsAPI*, const char* fileName);
	static Font LoadTTF(const GraphicsAPI*, MemoryArena* arena, const char* fileName);
	static AudioSample LoadWAV(const char* fileName);

	static void ReleaseFont(const GraphicsAPI*, Font*);
};