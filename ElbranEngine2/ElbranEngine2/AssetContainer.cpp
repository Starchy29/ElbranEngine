#include "AssetContainer.h"
#include "GraphicsAPI.h"
#include "Application.h"
#include "SoundMixer.h"
#include <stdint.h>

#define LoadSpriteSheet(fileName, rows, cols, numSprites) SpriteSheet{ graphics->LoadSprite(fileName), rows, cols, numSprites }

AssetContainer::AssetContainer(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio) {
	defaultSampler = graphics->CreateDefaultSampler();
	graphics->SetSampler(ShaderStage::Vertex, &defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Geometry, &defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Pixel, &defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Compute, &defaultSampler, 0);

	// create unit square
	Vertex vertices[] = {
		{ Vector2(-0.5f, -0.5f), Vector2(0.0f, 1.0f) },
		{ Vector2(-0.5f, 0.5f), Vector2(0.0f, 0.0f) },
		{ Vector2(0.5f, 0.5f), Vector2(1.0f, 0.0f) },
		{ Vector2(0.5f, -0.5f), Vector2(1.0f, 1.0f) }
	};

	uint32_t indices[] = {
		0, 1, 3, // clockwise winding order
		1, 2, 3
	};

	unitSquare = graphics->CreateMesh(vertices, 4, indices, 6, false);

	// create unit triangle
	Vertex triVerts[] = {
		{ Vector2(-0.5f, 0.5f), Vector2(0.0f, 0.0f) },
		{ Vector2(0.5f, 0.f), Vector2(1.0f, 0.5f) },
		{ Vector2(-0.5f, -0.5f), Vector2(0.0f, 1.0f) }
	};

	uint32_t triIndices[] = { 0, 1, 2 };
	unitTriangle = graphics->CreateMesh(triVerts, 3, triIndices, 3, false);

	// load shaders
	fullscreenVS = graphics->LoadVertexShader(L"FullscreenVS.cso");
	cameraVS = graphics->LoadVertexShader(L"CameraVS.cso");
	particlePassPS = graphics->LoadVertexShader(L"ParticlePassVS.cso");

	particleQuadGS = graphics->LoadGeometryShader(L"ParticleQuadGS.cso");

	solidColorPS = graphics->LoadPixelShader(L"SolidColorPS.cso");
	texturePS = graphics->LoadPixelShader(L"TexturePS.cso");
	circleFillPS = graphics->LoadPixelShader(L"CircleFillPS.cso");

	conSatValPP = graphics->LoadPixelShader(L"ConSatValPP.cso");
	blurPP = graphics->LoadPixelShader(L"BlurPP.cso");
	bloomFilterPP = graphics->LoadPixelShader(L"BloomFilterPP.cso");
	screenSumPP = graphics->LoadPixelShader(L"ScreenSumPP.cso");

	brightnessSumCS = graphics->LoadComputeShader(L"BrightnessSumCS.cso");
	particleSpawnCS = graphics->LoadComputeShader(L"ParticleSpawnCS.cso");
	particleMoveCS = graphics->LoadComputeShader(L"ParticleMoveCS.cso");
	textRasterizePS = graphics->LoadPixelShader(L"TextRasterizePS.cso");

	testSprite = graphics->LoadSprite(L"elbran.png");
	arial = Font::Load(L"arial.ttf");
}

AssetContainer::~AssetContainer() {
	GraphicsAPI* graphics = app->graphics;
	SoundMixer* audio = app->audio;

	graphics->ReleaseSampler(&defaultSampler);
	graphics->ReleaseMesh(&unitSquare);
	graphics->ReleaseMesh(&unitTriangle);

	graphics->ReleaseShader(&fullscreenVS);
	graphics->ReleaseShader(&cameraVS);
	graphics->ReleaseShader(&particlePassPS);

	graphics->ReleaseShader(&particleQuadGS);

	graphics->ReleaseShader(&solidColorPS);
	graphics->ReleaseShader(&texturePS);
	graphics->ReleaseShader(&circleFillPS);

	graphics->ReleaseShader(&conSatValPP);
	graphics->ReleaseShader(&blurPP);
	graphics->ReleaseShader(&bloomFilterPP);
	graphics->ReleaseShader(&screenSumPP);

	graphics->ReleaseShader(&brightnessSumCS);
	graphics->ReleaseShader(&particleSpawnCS);
	graphics->ReleaseShader(&particleMoveCS);
	graphics->ReleaseShader(&textRasterizePS);

	graphics->ReleaseTexture(&testSprite);

	arial.Release();
}