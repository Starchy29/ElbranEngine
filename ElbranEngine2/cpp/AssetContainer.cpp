#include "AssetContainer.h"
#include "GraphicsAPI.h"

void AssetContainer::Initialize(const GraphicsAPI* graphics, MemoryArena* arena) {
	defaultSampler = graphics->CreateDefaultSampler();
	graphics->SetSampler(ShaderStage::Vertex, defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Geometry, defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Pixel, defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Compute, defaultSampler, 0);

	// create unit square
	const Mesh::Vertex vertices[] = {
		{ Vector2(-0.5f, -0.5f), 0.f, Vector2(0.0f, 1.0f) },
		{ Vector2(-0.5f, 0.5f), 0.f, Vector2(0.0f, 0.0f) },
		{ Vector2(0.5f, 0.5f), 0.f, Vector2(1.0f, 0.0f) },
		{ Vector2(0.5f, -0.5f), 0.f, Vector2(1.0f, 1.0f) }
	};

	uint32_t indices[] = {
		0, 1, 3, // clockwise winding order
		1, 2, 3
	};

	unitSquare = graphics->CreateMesh(vertices, 4, indices, 6, false);

	// create unit triangle
	const Mesh::Vertex triVerts[] = {
		{ Vector2(-0.5f, 0.5f), 0.f, Vector2(0.0f, 0.0f) },
		{ Vector2(0.5f, 0.f), 0.f, Vector2(1.0f, 0.5f) },
		{ Vector2(-0.5f, -0.5f), 0.f, Vector2(0.0f, 1.0f) }
	};

	uint32_t triIndices[] = { 0, 1, 2 };
	unitTriangle = graphics->CreateMesh(triVerts, 3, triIndices, 3, false);

	// Load shaders
	LoadedFile packedShaders = FileIO::LoadFile("shaders.bin", 0);

	LoadedFile cameraShaderBlob = FileIO::UnpackFile(packedShaders, "CameraVS.cso");
	graphics->CreateDefaultInputLayout(cameraShaderBlob);
	shaders.cameraVS = graphics->CreateVertexShader(cameraShaderBlob);
	shaders.fullscreenVS = graphics->CreateVertexShader(FileIO::UnpackFile(packedShaders, "FullscreenVS.cso"));
	shaders.particlePassPS = graphics->CreateVertexShader(FileIO::UnpackFile(packedShaders, "ParticlePassVS.cso"));

	shaders.particleQuadGS = graphics->CreateGeometryShader(FileIO::UnpackFile(packedShaders, "ParticleQuadGS.cso"));

	shaders.solidColorPS = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "SolidColorPS.cso"));
	shaders.texturePS = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "TexturePS.cso"));
	shaders.atlasPS = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "AtlasPS.cso"));
	shaders.circleFillPS = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "CircleFillPS.cso"));
	shaders.textRasterizePS = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "TextRasterizePS.cso"));

	shaders.conSatValPP = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "ConSatValPS.cso"));
	shaders.blurPP = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "BlurPS.cso"));
	shaders.bloomFilterPP = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "BloomFilterPS.cso"));
	shaders.screenSumPP = graphics->CreatePixelShader(FileIO::UnpackFile(packedShaders, "ScreenSumPS.cso"));

	shaders.brightnessSumCS = graphics->CreateComputeShader(FileIO::UnpackFile(packedShaders, "BrightnessSumCS.cso"));
	shaders.particleSpawnCS = graphics->CreateComputeShader(FileIO::UnpackFile(packedShaders, "ParticleSpawnCS.cso"));
	shaders.particleMoveCS = graphics->CreateComputeShader(FileIO::UnpackFile(packedShaders, "ParticleMoveCS.cso"));
	shaders.particleClearCS = graphics->CreateComputeShader(FileIO::UnpackFile(packedShaders, "ParticleClearCS.cso"));

	// Load assets
	LoadedFile packedAssets = FileIO::LoadFile("game_assets.bin", 0);

	sprites.testSprite = graphics->CreateSprite(FileIO::LoadPNG(FileIO::UnpackFile(packedAssets, "elbran.png")));
	fonts.arial = FileIO::LoadTTF(FileIO::UnpackFile(packedAssets, "arial.ttf"), graphics, arena);

	packedShaders.Release();
	packedAssets.Release();
}

void AssetContainer::Release(const GraphicsAPI* graphics) {
	graphics->ReleaseSampler(defaultSampler);
	graphics->ReleaseMesh(&unitSquare);
	graphics->ReleaseMesh(&unitTriangle);

	graphics->ReleaseShader(&shaders.fullscreenVS);
	graphics->ReleaseShader(&shaders.cameraVS);
	graphics->ReleaseShader(&shaders.particlePassPS);

	graphics->ReleaseShader(&shaders.particleQuadGS);

	graphics->ReleaseShader(&shaders.solidColorPS);
	graphics->ReleaseShader(&shaders.texturePS);
	graphics->ReleaseShader(&shaders.atlasPS);
	graphics->ReleaseShader(&shaders.circleFillPS);
	graphics->ReleaseShader(&shaders.textRasterizePS);

	graphics->ReleaseShader(&shaders.conSatValPP);
	graphics->ReleaseShader(&shaders.blurPP);
	graphics->ReleaseShader(&shaders.bloomFilterPP);
	graphics->ReleaseShader(&shaders.screenSumPP);

	graphics->ReleaseShader(&shaders.brightnessSumCS);
	graphics->ReleaseShader(&shaders.particleSpawnCS);
	graphics->ReleaseShader(&shaders.particleMoveCS);
	graphics->ReleaseShader(&shaders.particleClearCS);

	graphics->ReleaseTexture(&sprites.testSprite.texture);

	ReleaseFont(graphics, &fonts.arial);
}

void AssetContainer::ReleaseFont(const GraphicsAPI* graphics, Font* font) {
	graphics->ReleaseArrayBuffer(&font->glyphCurves);
	graphics->ReleaseArrayBuffer(&font->firstCurveIndices);
	delete[] font->glyphBaselines;
	delete[] font->glyphDimensions;
	font->charToGlyphIndex.Release();
}