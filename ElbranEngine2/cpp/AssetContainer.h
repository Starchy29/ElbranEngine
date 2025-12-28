#pragma once
#include "GraphicsData.h"
#include "AudioData.h"

class GraphicsAPI;
class MemoryArena;

struct ByteColor {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

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

	static VertexShader LoadVertexShader(const GraphicsAPI*, MemoryArena*, const char* fileName);
	static GeometryShader LoadGeometryShader(const GraphicsAPI*, MemoryArena*, const char* fileName);
	static PixelShader LoadPixelShader(const GraphicsAPI*, MemoryArena*, const char* fileName) ;
	static ComputeShader LoadComputeShader(const GraphicsAPI*, MemoryArena*, const char* fileName);

	static Sprite LoadSprite(const GraphicsAPI*, MemoryArena*, const char* fileName);
	static SpriteSheet LoadSpriteSheet(const GraphicsAPI*, MemoryArena*, const char* fileName, uint8_t rows, uint8_t cols);
	static void LoadBMP(const char* fileName, MemoryArena*, ByteColor** outColors, UInt2* outDimensions);
	static void LoadPNG(const char* fileName, MemoryArena*, ByteColor** outColors, UInt2* outDimensions);
	static Font LoadTTF(const GraphicsAPI*, MemoryArena* arena, const char* fileName);
	static AudioSample LoadWAV(const char* fileName, MemoryArena*);

	static void ReleaseFont(const GraphicsAPI*, Font*);
};