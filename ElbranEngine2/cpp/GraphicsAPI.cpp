#include "GraphicsAPI.h"
#include "Application.h"
#include "Lights.h"

void GraphicsAPI::Initialize() {
	projectionBuffer = CreateConstantBuffer(sizeof(Matrix));
	lightInfoBuffer = CreateConstantBuffer(sizeof(LightConstants));
	lightsBuffer = CreateArrayBuffer(ShaderDataType::Structured, MAX_LIGHTS_ONSCREEN, sizeof(LightData));
	totalBrightnessBuffer = CreateOutputBuffer(ShaderDataType::UInt, 4); // should match BrightnessSumCS.hlsl
}

void GraphicsAPI::Release() {
	ReleaseRenderTarget(&postProcessTargets[0]);
	ReleaseRenderTarget(&postProcessTargets[1]);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		ReleaseRenderTarget(&postProcessHelpers[i]);
	}

	ReleaseConstantBuffer(&projectionBuffer);
	ReleaseConstantBuffer(&lightInfoBuffer);
	ReleaseArrayBuffer(&lightsBuffer);
	ReleaseOuputBuffer(&totalBrightnessBuffer);
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

float GraphicsAPI::GetViewAspectRatio() const {
	return viewAspectRatio;
}

void GraphicsAPI::DrawFullscreen() {
	SetVertexShader(&app->assets.fullscreenVS);
	DrawVertices(3); // fullscreen triangle
}

void GraphicsAPI::SetVertexShader(const VertexShader* shader, const void* constantInput, uint32_t inputBytes) {
	WriteBuffer(constantInput, inputBytes, shader->constants.data);
	SetConstants(ShaderStage::Vertex, &shader->constants, OBJECT_CONSTANT_REGISTER);
	SetVertexShader(shader);
}

void GraphicsAPI::SetGeometryShader(const GeometryShader* shader, const void* constantInput, uint32_t inputBytes) {
	WriteBuffer(constantInput, inputBytes, shader->constants.data);
	SetConstants(ShaderStage::Geometry, &shader->constants, OBJECT_CONSTANT_REGISTER);
	SetGeometryShader(shader);
}

void GraphicsAPI::SetPixelShader(const PixelShader* shader, const void* constantInput, uint32_t inputBytes) {
	WriteBuffer(constantInput, inputBytes, shader->constants.data);
	SetConstants(ShaderStage::Pixel, &shader->constants, OBJECT_CONSTANT_REGISTER);
	SetPixelShader(shader);
}
