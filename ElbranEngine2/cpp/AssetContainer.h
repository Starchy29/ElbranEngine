#pragma once
#include "GraphicsData.h"
#include "AudioData.h"
#include "FileIO.h"

class GraphicsAPI;
class MemoryArena;

class AssetContainer {
public:
	Sampler* defaultSampler;
	Mesh unitSquare;
	Mesh unitTriangle;

	VertexShader fullscreenVS;
	VertexShader cameraVS;
	VertexShader particlePassPS;

	GeometryShader particleQuadGS;

	PixelShader solidColorPS;
	PixelShader texturePS;
	PixelShader atlasPS;
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

	Sprite testSprite;
	Font arial;

	AssetContainer() = default;
	void Initialize(GraphicsAPI*, MemoryArena*);
	void Release(GraphicsAPI*);

	static LoadedFile UnpackFile(LoadedFile loadedPackFile, const char* fileName);

	static ImageBuffer LoadBMP(LoadedFile, MemoryArena*);
	static ImageBuffer LoadPNG(LoadedFile, MemoryArena*);
	static Font LoadTTF(LoadedFile, const GraphicsAPI*, MemoryArena* arena);
	static AudioSample LoadWAV(LoadedFile, MemoryArena*);

	static void ReleaseFont(const GraphicsAPI*, Font*);
};