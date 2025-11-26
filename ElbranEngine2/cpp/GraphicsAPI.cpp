#include "GraphicsAPI.h"
#include "Application.h"
#include "Lights.h"

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

	ReleaseConstantBuffer(&projectionBuffer);
	ReleaseConstantBuffer(&lightInfoBuffer);
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

void GraphicsAPI::ApplyPostProcesses(const PostProcess* postProcessSequence, uint8_t ppCount) {
	for(uint32_t i = 0; i < ppCount; i++) {
		if(!postProcessSequence[i].IsActive()) continue;
		RenderTarget* input = &postProcessTargets[renderTargetIndex];
		renderTargetIndex = 1 - renderTargetIndex;
		RenderTarget* output = &postProcessTargets[renderTargetIndex];
		postProcessSequence[i].Render(input, output, this, &app->assets);
	}

	SetRenderTarget(&postProcessTargets[renderTargetIndex], true);
}

UInt2 GraphicsAPI::GetViewDimensions() const {
	return viewportDims;
}

UInt2 GraphicsAPI::GetViewOffset() const {
	return viewportOffset;
}

void GraphicsAPI::DrawFullscreen() {
	SetVertexShader(&app->assets.fullscreenVS);
	DrawVertices(3); // fullscreen triangle
}

void GraphicsAPI::SetVertexShader(const VertexShader* shader, const void* constantInput, uint32_t inputBytes) {
	if(constantInput) {
		WriteBuffer(constantInput, inputBytes, shader->constants.data);
		SetConstants(ShaderStage::Vertex, &shader->constants, OBJECT_CONSTANT_REGISTER);
	}
	platformGraphics->SetVertexShader(shader);
}

void GraphicsAPI::SetGeometryShader(const GeometryShader* shader, const void* constantInput, uint32_t inputBytes) {
	if(constantInput) {
		WriteBuffer(constantInput, inputBytes, shader->constants.data);
		SetConstants(ShaderStage::Geometry, &shader->constants, OBJECT_CONSTANT_REGISTER);
	}
	platformGraphics->SetGeometryShader(shader);
}

void GraphicsAPI::SetPixelShader(const PixelShader* shader, const void* constantInput, uint32_t inputBytes) {
	if(constantInput) {
		WriteBuffer(constantInput, inputBytes, shader->constants.data);
		SetConstants(ShaderStage::Pixel, &shader->constants, OBJECT_CONSTANT_REGISTER);
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
	platformGraphics->CopyTexture(&postProcessTargets[renderTargetIndex], &backBuffer);
	platformGraphics->PresentFrame();
}

bool GraphicsAPI::IsFullscreen() const { return platformGraphics->IsFullscreen(); }
void GraphicsAPI::SetFullscreen(bool fullscreen) { platformGraphics->SetFullscreen(fullscreen); }

VertexShader GraphicsAPI::LoadVertexShader(std::wstring fileName) {
	LoadedFile shaderBlob = app->LoadFile(L"shaders\\" + fileName);
	VertexShader result = platformGraphics->CreateVertexShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

GeometryShader GraphicsAPI::LoadGeometryShader(std::wstring fileName) {
	LoadedFile shaderBlob = app->LoadFile(L"shaders\\" + fileName);
	GeometryShader result = platformGraphics->CreateGeometryShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

PixelShader GraphicsAPI::LoadPixelShader(std::wstring fileName) {
	LoadedFile shaderBlob = app->LoadFile(L"shaders\\" + fileName);
	PixelShader result = platformGraphics->CreatePixelShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

ComputeShader GraphicsAPI::LoadComputeShader(std::wstring fileName) {
	LoadedFile shaderBlob = app->LoadFile(L"shaders\\" + fileName);
	ComputeShader result = platformGraphics->CreateComputeShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

Texture2D GraphicsAPI::CreateConstantTexture(uint32_t width, uint32_t height, uint8_t* textureData) { return platformGraphics->CreateConstantTexture(width, height, textureData); }
Sampler GraphicsAPI::CreateDefaultSampler() { return platformGraphics->CreateDefaultSampler(); }
Mesh GraphicsAPI::CreateMesh(const Vertex* vertices, uint16_t vertexCount, const uint32_t* indices, uint16_t indexCount, bool editable) { return platformGraphics->CreateMesh(vertices, vertexCount, indices, indexCount, editable); }
ConstantBuffer GraphicsAPI::CreateConstantBuffer(uint32_t byteLength) { return platformGraphics->CreateConstantBuffer(byteLength); }
ArrayBuffer GraphicsAPI::CreateArrayBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes) { return platformGraphics->CreateArrayBuffer(type, elements, structBytes); }
EditBuffer GraphicsAPI::CreateEditBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes) { return platformGraphics->CreateEditBuffer(type, elements, structBytes); }
OutputBuffer GraphicsAPI::CreateOutputBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes) { return platformGraphics->CreateOutputBuffer(type, elements, structBytes); }
RenderTarget GraphicsAPI::CreateRenderTarget(uint32_t width, uint32_t height) { return platformGraphics->CreateRenderTarget(width, height); }
ComputeTexture GraphicsAPI::CreateComputeTexture(uint32_t width, uint32_t height) { return platformGraphics->CreateComputeTexture(width, height); }

void GraphicsAPI::SetBlendMode(BlendState mode) { platformGraphics->SetBlendMode(mode); }
void GraphicsAPI::SetPrimitive(RenderPrimitive primitive) { platformGraphics->SetPrimitive(primitive); }
void GraphicsAPI::SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil) { platformGraphics->SetRenderTarget(renderTarget, useDepthStencil); }
void GraphicsAPI::DrawVertices(uint16_t numVertices) { platformGraphics->DrawVertices(numVertices); }
void GraphicsAPI::DrawMesh(const Mesh* mesh) { platformGraphics->DrawMesh(mesh); }

void GraphicsAPI::SetComputeTexture(const ComputeTexture* texture, uint8_t slot) { platformGraphics->SetComputeTexture(texture, slot); }
void GraphicsAPI::SetEditBuffer(EditBuffer* buffer, uint8_t slot) { platformGraphics->SetEditBuffer(buffer, slot); }
void GraphicsAPI::SetOutputBuffer(OutputBuffer* buffer, uint8_t slot, const void* initialData) { platformGraphics->SetOutputBuffer(buffer, slot, initialData); }
void GraphicsAPI::ReadBuffer(const OutputBuffer* buffer, void* destination) { platformGraphics->ReadBuffer(buffer, destination); }
void GraphicsAPI::RunComputeShader(const ComputeShader* shader, uint16_t xThreads, uint16_t yThreads, uint16_t zThreads) { platformGraphics->RunComputeShader(shader, xThreads, yThreads, zThreads); }

void GraphicsAPI::ReleaseShader(VertexShader* shader) { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseShader(GeometryShader* shader) { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseShader(PixelShader* shader) { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseShader(ComputeShader* shader) { platformGraphics->ReleaseShader(shader); }
void GraphicsAPI::ReleaseSampler(Sampler* sampler) { platformGraphics->ReleaseSampler(sampler); }
void GraphicsAPI::ReleaseTexture(Texture2D* texture) { platformGraphics->ReleaseTexture(texture); }
void GraphicsAPI::ReleaseComputeTexture(ComputeTexture* texture) { platformGraphics->ReleaseComputeTexture(texture); }
void GraphicsAPI::ReleaseMesh(Mesh* mesh) { platformGraphics->ReleaseMesh(mesh); }
void GraphicsAPI::ReleaseConstantBuffer(ConstantBuffer* buffer) { platformGraphics->ReleaseConstantBuffer(buffer); }
void GraphicsAPI::ReleaseArrayBuffer(ArrayBuffer* buffer) { platformGraphics->ReleaseArrayBuffer(buffer); }
void GraphicsAPI::ReleaseEditBuffer(EditBuffer* buffer) { platformGraphics->ReleaseEditBuffer(buffer); }
void GraphicsAPI::ReleaseOuputBuffer(OutputBuffer* buffer) { platformGraphics->ReleaseOuputBuffer(buffer); }

void GraphicsAPI::WriteBuffer(const void* data, uint32_t byteLength, Buffer* buffer) { platformGraphics->WriteBuffer(data, byteLength, buffer); }
void GraphicsAPI::SetConstants(ShaderStage stage, const ConstantBuffer* buffer, uint8_t slot) { platformGraphics->SetConstants(stage, buffer, slot); }
void GraphicsAPI::SetArray(ShaderStage stage, const ArrayBuffer* buffer, uint8_t slot) { platformGraphics->SetArray(stage, buffer, slot); }
void GraphicsAPI::SetTexture(ShaderStage stage, const Texture2D* texture, uint8_t slot) { platformGraphics->SetTexture(stage, texture, slot); }
void GraphicsAPI::SetSampler(ShaderStage stage, Sampler* sampler, uint8_t slot) { platformGraphics->SetSampler(stage, sampler, slot); }
void GraphicsAPI::CopyTexture(Texture2D* source, Texture2D* destination) { platformGraphics->CopyTexture(source, destination); }
void GraphicsAPI::ClearMesh() { platformGraphics->ClearMesh(); }
