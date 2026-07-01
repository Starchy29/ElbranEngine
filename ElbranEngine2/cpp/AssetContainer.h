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

	struct {
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
	} shaders;

	struct {
		Sprite testSprite;
	} sprites;
	
	struct {
		Font arial;
	} fonts;

	struct {
	
	} audio;

	AssetContainer() = default;
	void Initialize(const GraphicsAPI*, MemoryArena*);
	void Release(const GraphicsAPI*);

	static void ReleaseFont(const GraphicsAPI*, Font*);
};