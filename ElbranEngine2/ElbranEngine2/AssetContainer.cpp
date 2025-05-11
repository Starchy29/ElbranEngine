#include "AssetContainer.h"
#include "GraphicsAPI.h"
#include "Application.h"
#include "SoundMixer.h"

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

	unsigned int indices[] = {
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

	unsigned int triIndices[] = { 0, 1, 2 };
	unitTriangle = graphics->CreateMesh(triVerts, 3, triIndices, 3, false);

	// load shaders
	fullscreenVS = graphics->LoadVertexShader(filePath, L"FullscreenVS.cso");
	cameraVS = graphics->LoadVertexShader(filePath, L"CameraVS.cso");
	particlePassPS = graphics->LoadVertexShader(filePath, L"ParticlePassVS.cso");

	particleQuadGS = graphics->LoadGeometryShader(filePath, L"ParticleQuadGS.cso");

	solidColorPS = graphics->LoadPixelShader(filePath, L"SolidColorPS.cso");
	texturePS = graphics->LoadPixelShader(filePath, L"TexturePS.cso");
	circleFillPS = graphics->LoadPixelShader(filePath, L"CircleFillPS.cso");

	conSatValPP = graphics->LoadPixelShader(filePath, L"ConSatValPP.cso");
	blurPP = graphics->LoadPixelShader(filePath, L"BlurPP.cso");
	bloomFilterPP = graphics->LoadPixelShader(filePath, L"BloomFilterPP.cso");
	screenSumPP = graphics->LoadPixelShader(filePath, L"ScreenSumPP.cso");

	brightnessSumCS = graphics->LoadComputeShader(filePath, L"BrightnessSumCS.cso");
	particleSpawnCS = graphics->LoadComputeShader(filePath, L"ParticleSpawnCS.cso");
	particleMoveCS = graphics->LoadComputeShader(filePath, L"ParticleMoveCS.cso");

	testSprite = graphics->LoadSprite(filePath, L"elbran.png");
	apple = graphics->LoadSprite(filePath, L"apple.jpeg");

	testSound = audio->LoadEffect(filePath, L"water plunk.wav");
	testMusic = audio->LoadTrack(filePath, L"Menu song.wav", true);
}

AssetContainer::~AssetContainer() {
	//GraphicsAPI* graphics = app->graphics;
	SoundMixer* audio = app->audio;

	defaultSampler.Release();
	unitSquare.Release();
	unitTriangle.Release();

	fullscreenVS.Release();
	cameraVS.Release();
	particlePassPS.Release();

	particleQuadGS.Release();

	solidColorPS.Release();
	texturePS.Release();
	circleFillPS.Release();

	conSatValPP.Release();
	blurPP.Release();
	bloomFilterPP.Release();
	screenSumPP.Release();

	brightnessSumCS.Release();
	particleSpawnCS.Release();
	particleMoveCS.Release();

	testSprite.Release();
	apple.Release();

	audio->ReleaseSoundEffect(&testSound);
	audio->ReleaseAudioTrack(&testMusic);
}
