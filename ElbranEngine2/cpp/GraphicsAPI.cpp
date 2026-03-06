#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "PostProcess.h"

#ifdef WINDOWS
#include "DirectXAPI.h"
#endif

void GraphicsAPI::Initialize(PlatformGraphics* platformGraphics, UInt2 windowSize) {
	this->platformGraphics = platformGraphics;
	projectionBuffer = CreateConstantBuffer(sizeof(Matrix));
	lightInfoBuffer = CreateConstantBuffer(sizeof(LightConstants));
	lightsBuffer = CreateArrayBuffer(ShaderDataType::Structured, MAX_LIGHTS_ONSCREEN, sizeof(LightData));
	totalBrightnessBuffer = CreateOutputBuffer(ShaderDataType::UInt, 4); // should match BrightnessSumCS

	// create render targets
	ResizeScreen(windowSize);
}

void GraphicsAPI::Release() {
	platformGraphics->ReleaseRenderTarget(&postProcessTargets[0]);
	platformGraphics->ReleaseRenderTarget(&postProcessTargets[1]);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		platformGraphics->ReleaseRenderTarget(&postProcessHelpers[i]);
	}
	platformGraphics->ReleaseRenderTarget(&backBuffer);

	ReleaseBuffer(projectionBuffer);
	ReleaseBuffer(lightInfoBuffer);
	ReleaseArrayBuffer(&lightsBuffer);
	ReleaseOuputBuffer(&totalBrightnessBuffer);
	delete platformGraphics;
}

void GraphicsAPI::ResizeScreen(UInt2 windowSize) {
	if(!platformGraphics) return; // platform might call this before initialization

	// adjust the view offset to fill the screen while maintaining the aspect ratio
	float windowAspectRatio = (float)windowSize.x / windowSize.y;
	viewportOffset = UInt2(0, 0);
	viewportDims = UInt2(windowSize.x, windowSize.y);
	if(windowAspectRatio > ASPECT_RATIO) {
		viewportDims.x = windowSize.y * ASPECT_RATIO;
		viewportOffset.x = (windowSize.x - viewportDims.x) / 2;
	} else {
		viewportDims.y = windowSize.x / ASPECT_RATIO;
		viewportOffset.y = (windowSize.y - viewportDims.y) / 2;
	}

	// recreate render targets
	platformGraphics->ReleaseRenderTarget(&postProcessTargets[0]);
	platformGraphics->ReleaseRenderTarget(&postProcessTargets[1]);
	postProcessTargets[0] = platformGraphics->CreateRenderTarget(windowSize.x, windowSize.y);
	postProcessTargets[1] = platformGraphics->CreateRenderTarget(windowSize.x, windowSize.y);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		platformGraphics->ReleaseRenderTarget(&postProcessHelpers[i]);
		postProcessHelpers[i] = platformGraphics->CreateRenderTarget(windowSize.x, windowSize.y);
	}
	platformGraphics->ReleaseRenderTarget(&backBuffer);
	platformGraphics->Resize(windowSize, viewportDims, viewportOffset);
	backBuffer = platformGraphics->GetBackBuffer();
	SetRenderTarget(&postProcessTargets[renderTargetIndex], true);
}

void GraphicsAPI::ApplyPostProcesses(const AssetContainer* assets, const PostProcess* postProcessSequence, uint8_t ppCount) {
	for(uint32_t i = 0; i < ppCount; i++) {
		if(!postProcessSequence[i].IsActive()) continue;
		RenderTarget* input = &postProcessTargets[renderTargetIndex];
		renderTargetIndex = 1 - renderTargetIndex;
		RenderTarget* output = &postProcessTargets[renderTargetIndex];
		postProcessSequence[i].Render(input, output, this, assets);
	}

	SetRenderTarget(&postProcessTargets[renderTargetIndex], true);
}

void GraphicsAPI::DrawFullscreen(const AssetContainer* assets) {
	SetVertexShader(&assets->fullscreenVS);
	DrawVertices(3); // fullscreen triangle
}

void GraphicsAPI::SetVertexShader(const VertexShader* shader, const void* constantInput, uint32_t inputBytes) {
	if(constantInput) {
		WriteBuffer(constantInput, inputBytes, shader->constants);
		SetConstants(ShaderStage::Vertex, shader->constants, OBJECT_CONSTANT_REGISTER);
	}
	platformGraphics->SetVertexShader(shader);
}

void GraphicsAPI::SetGeometryShader(const GeometryShader* shader, const void* constantInput, uint32_t inputBytes) {
	if(constantInput) {
		WriteBuffer(constantInput, inputBytes, shader->constants);
		SetConstants(ShaderStage::Geometry, shader->constants, OBJECT_CONSTANT_REGISTER);
	}
	platformGraphics->SetGeometryShader(shader);
}

void GraphicsAPI::SetPixelShader(const PixelShader* shader, const void* constantInput, uint32_t inputBytes) {
	if(constantInput) {
		WriteBuffer(constantInput, inputBytes, shader->constants);
		SetConstants(ShaderStage::Pixel, shader->constants, OBJECT_CONSTANT_REGISTER);
	}
	platformGraphics->SetPixelShader(shader);
}

void GraphicsAPI::ResetRenderTargets() {
	platformGraphics->ClearDepthStencil();

	platformGraphics->ClearRenderTarget(&backBuffer);
	platformGraphics->ClearRenderTarget(&postProcessTargets[0]);
	platformGraphics->ClearRenderTarget(&postProcessTargets[1]);
	for(uint8_t i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		platformGraphics->ClearRenderTarget(&postProcessHelpers[i]);
	}

	renderTargetIndex = 0;
	SetRenderTarget(&postProcessTargets[renderTargetIndex], true);
}

void GraphicsAPI::PresentFrame() {
	platformGraphics->CopyTexture(&postProcessTargets[renderTargetIndex].texture, &backBuffer.texture);
	platformGraphics->PresentFrame();
}

Sprite GraphicsAPI::CreateSprite(ImageBuffer image) const {
	Sprite result = {};
	result.texture = CreateConstantTexture(image.width, image.height, (uint8_t*)image.pixels);
	for(uint32_t i = 0; i < image.width * image.height; i++) {
		if(image.pixels[i].alpha > 0 && image.pixels[i].alpha < 255)  {
			result.translucent = true;
			break;
		}
	}
	return result;
}

SpriteSheet GraphicsAPI::CreateSpriteSheet(MemoryArena* arena, ImageBuffer image, uint16_t rows, uint16_t cols) const {
	SpriteSheet result = {};
	result.rows = rows;
	result.cols = cols;

	// reorder pixels
	uint32_t elementWidth = image.width / cols;
	uint32_t elementHeight = image.height / rows;
	uint32_t pixelsPerElement = elementHeight * elementWidth;
	uint32_t elements = (uint32_t)rows * cols;

	ImageBuffer::Pixel* reorderedPixels;
	if(arena) reorderedPixels = (ImageBuffer::Pixel*)arena->Reserve(sizeof(ImageBuffer::Pixel) * image.width * image.height);
	else reorderedPixels = new ImageBuffer::Pixel[image.width * image.height];

	for(uint8_t r = 0; r < rows; r++) {
		for(uint8_t c = 0; c < cols; c++) {
			for(uint32_t h = 0; h < elementHeight; h++) {
				memcpy(reorderedPixels + c * pixelsPerElement + r * cols * pixelsPerElement + h * elementWidth,
					image.pixels + c * elementWidth + r * image.width * elementHeight + h * image.width, 
					elementWidth * sizeof(ImageBuffer::Pixel)
				);
			}
		}
	}

	result.textures = CreateTextureArray((uint8_t*)reorderedPixels, elements, elementWidth, elementHeight);

	// check for translucency
	for(uint32_t i = 0; i < image.width * image.height; i++) {
		if(image.pixels[i].alpha > 0 && image.pixels[i].alpha < 255)  {
			result.translucent = true;
			break;
		}
	}

	if(!arena) delete[] reorderedPixels;
	return result;
}

Mesh GraphicsAPI::CreateMesh(const Mesh::Vertex* vertices, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount, bool editable) const {
	Mesh result = {};
	result.vertexCount = vertexCount;
	result.indexCount = indexCount;
	result.vertices = platformGraphics->CreateVertexBuffer(vertices, vertexCount, editable);
	result.indices = platformGraphics->CreateIndexBuffer(indices, indexCount);
	return result;
}

void GraphicsAPI::DrawMesh(const Mesh* mesh) {
	platformGraphics->SetVertexBuffer(mesh->vertices);
	platformGraphics->SetIndexBuffer(mesh->indices);
	platformGraphics->DrawIndices(mesh->indexCount);
}

bool GraphicsAPI::IsFullscreen() const { return platformGraphics->IsFullscreen(); }
void GraphicsAPI::SetFullscreen(bool fullscreen) { platformGraphics->SetFullscreen(fullscreen); }

VertexShader GraphicsAPI::CreateVertexShader(LoadedFile shaderBlob) const { return platformGraphics->CreateVertexShader(shaderBlob); }
GeometryShader GraphicsAPI::CreateGeometryShader(LoadedFile shaderBlob) const { return platformGraphics->CreateGeometryShader(shaderBlob); }
PixelShader GraphicsAPI::CreatePixelShader(LoadedFile shaderBlob) const { return platformGraphics->CreatePixelShader(shaderBlob); }
ComputeShader GraphicsAPI::CreateComputeShader(LoadedFile shaderBlob) const { return platformGraphics->CreateComputeShader(shaderBlob); }

Texture2D GraphicsAPI::CreateConstantTexture(uint32_t width, uint32_t height, const uint8_t* textureData) const { return platformGraphics->CreateConstantTexture(width, height, textureData); }
Texture2DArray GraphicsAPI::CreateTextureArray(const uint8_t* textureData, uint16_t numElements, uint32_t textureWidth, uint32_t textureHeight) const { return platformGraphics->CreateTextureArray(textureData, numElements, textureWidth, textureHeight); }
Sampler* GraphicsAPI::CreateDefaultSampler() const { return platformGraphics->CreateDefaultSampler(); }
void GraphicsAPI::CreateDefaultInputLayout(LoadedFile vertexShaderBlob) { platformGraphics->CreateDefaultInputLayout(vertexShaderBlob); }
GraphicsBuffer* GraphicsAPI::CreateVertexBuffer(const Mesh::Vertex* vertices, uint32_t vertexCount, bool editable) const { return platformGraphics->CreateVertexBuffer(vertices, vertexCount, editable); }
GraphicsBuffer* GraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t indexCount) const { return platformGraphics->CreateIndexBuffer(indices, indexCount); }
GraphicsBuffer* GraphicsAPI::CreateConstantBuffer(uint32_t byteLength) const { return platformGraphics->CreateConstantBuffer(byteLength); }
ArrayBuffer GraphicsAPI::CreateArrayBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes) const{ return platformGraphics->CreateArrayBuffer(type, elements, structBytes); }
EditBuffer GraphicsAPI::CreateEditBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes) const { return platformGraphics->CreateEditBuffer(type, elements, structBytes); }
OutputBuffer GraphicsAPI::CreateOutputBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes) const { return platformGraphics->CreateOutputBuffer(type, elements, structBytes); }
RenderTarget GraphicsAPI::CreateRenderTarget(uint32_t width, uint32_t height) const { return platformGraphics->CreateRenderTarget(width, height); }
ComputeTexture GraphicsAPI::CreateComputeTexture(uint32_t width, uint32_t height) const { return platformGraphics->CreateComputeTexture(width, height); }

void GraphicsAPI::SetBlendMode(BlendState mode) { platformGraphics->SetBlendMode(mode); }
void GraphicsAPI::SetPrimitive(RenderPrimitive primitive) { platformGraphics->SetPrimitive(primitive); }
void GraphicsAPI::SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil) { platformGraphics->SetRenderTarget(renderTarget, useDepthStencil); }
void GraphicsAPI::SetVertexBuffer(GraphicsBuffer* vertices) { platformGraphics->SetVertexBuffer(vertices); }
void GraphicsAPI::SetIndexBuffer(GraphicsBuffer* indices) { platformGraphics->SetIndexBuffer(indices); }
void GraphicsAPI::DrawVertices(uint32_t numVertices) { platformGraphics->DrawVertices(numVertices); }
void GraphicsAPI::DrawIndices(uint32_t numIndices) { platformGraphics->DrawIndices(numIndices); }

void GraphicsAPI::SetComputeTexture(const ComputeTexture* texture, uint8_t slot) { platformGraphics->SetComputeTexture(texture, slot); }
void GraphicsAPI::SetEditBuffer(const EditBuffer* buffer, uint8_t slot) { platformGraphics->SetEditBuffer(buffer, slot); }
void GraphicsAPI::SetOutputBuffer(const OutputBuffer* buffer, uint8_t slot, const void* initialData) { platformGraphics->SetOutputBuffer(buffer, slot, initialData); }
void GraphicsAPI::ReadBuffer(const OutputBuffer* buffer, void* destination) const { platformGraphics->ReadBuffer(buffer, destination); }
void GraphicsAPI::RunComputeShader(const ComputeShader* shader, uint16_t xThreads, uint16_t yThreads, uint16_t zThreads) const { platformGraphics->RunComputeShader(shader, xThreads, yThreads, zThreads); }

void GraphicsAPI::ReleaseShader(VertexShader* shader) const { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseShader(GeometryShader* shader) const { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseShader(PixelShader* shader) const { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseShader(ComputeShader* shader) const { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseSampler(Sampler* sampler) const { platformGraphics->ReleaseSampler(sampler); }
void GraphicsAPI::ReleaseTexture(Texture2D* texture) const { platformGraphics->ReleaseTexture(texture); }
void GraphicsAPI::ReleaseTextureArray(Texture2DArray* textures) const { platformGraphics->ReleaseTextureArray(textures); }
void GraphicsAPI::ReleaseComputeTexture(ComputeTexture* texture) const { platformGraphics->ReleaseComputeTexture(texture); }
void GraphicsAPI::ReleaseMesh(Mesh* mesh) const { platformGraphics->ReleaseMesh(mesh); }
void GraphicsAPI::ReleaseBuffer(GraphicsBuffer* buffer) const { platformGraphics->ReleaseBuffer(buffer); }
void GraphicsAPI::ReleaseArrayBuffer(ArrayBuffer* buffer) const { platformGraphics->ReleaseArrayBuffer(buffer); }
void GraphicsAPI::ReleaseEditBuffer(EditBuffer* buffer) const { platformGraphics->ReleaseEditBuffer(buffer); }
void GraphicsAPI::ReleaseOuputBuffer(OutputBuffer* buffer) const { platformGraphics->ReleaseOuputBuffer(buffer); }

void GraphicsAPI::WriteBuffer(const void* data, uint32_t byteLength, GraphicsBuffer* buffer) const { platformGraphics->WriteBuffer(data, byteLength, buffer); }
void GraphicsAPI::SetConstants(ShaderStage stage, GraphicsBuffer* buffer, uint8_t slot) { platformGraphics->SetConstants(stage, buffer, slot); }
void GraphicsAPI::SetArray(ShaderStage stage, const ArrayBuffer* buffer, uint8_t slot) { platformGraphics->SetArray(stage, buffer, slot); }
void GraphicsAPI::SetTexture(ShaderStage stage, const Texture2D* texture, uint8_t slot) { platformGraphics->SetTexture(stage, texture, slot); }
void GraphicsAPI::SetTextureArray(ShaderStage stage, const Texture2DArray* textures, uint8_t slot) { platformGraphics->SetTextureArray(stage, textures, slot); }
void GraphicsAPI::SetSampler(ShaderStage stage, Sampler* sampler, uint8_t slot) { platformGraphics->SetSampler(stage, sampler, slot); }
void GraphicsAPI::CopyTexture(const Texture2D* source, Texture2D* destination) const { platformGraphics->CopyTexture(source, destination); }
void GraphicsAPI::ClearMesh() { platformGraphics->ClearMesh(); }
