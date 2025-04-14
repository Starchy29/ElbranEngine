#include "AssetContainer.h"
#include "GraphicsAPI.h"
#include "Application.h"

AssetContainer::AssetContainer(std::wstring filePath, GraphicsAPI* graphics) {
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

	// load shaders
	fullscreenVS = graphics->LoadVertexShader(filePath, L"FullscreenVS.cso");
	cameraVS = graphics->LoadVertexShader(filePath, L"CameraVS.cso");

	solidColorPS = graphics->LoadPixelShader(filePath, L"SolidColorPS.cso");
	texturePS = graphics->LoadPixelShader(filePath, L"TexturePS.cso");

	testSprite = graphics->LoadSprite(filePath, L"elbran.png");
}

AssetContainer::~AssetContainer() {
	GraphicsAPI* graphics = app->graphics;

	unitSquare.Release();
	defaultSampler.Release();

	fullscreenVS.Release();
	cameraVS.Release();

	solidColorPS.Release();
	texturePS.Release();

	testSprite.Release();
}
