#include "GraphicsAPI.h"
#include "Application.h"
#include <cassert>

GraphicsAPI::GraphicsAPI() :
	postProcessed{false},
	viewportDims{0,0},
	viewportOffset{0,0},
	viewAspectRatio{0.0f}
{
	postProcessTargets[0] = {};
	postProcessTargets[1] = {};
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		postProcessHelpers[i] = {};
	}
}

void GraphicsAPI::Render(Game* game) {
	postProcessed = false;
	ClearBackBuffer();
	ClearDepthStencil();

	uint32_t numPostProcesses = postProcesses.Size();
	if(numPostProcesses > 0) {
		SetRenderTarget(&postProcessTargets[0], true);
	}

	game->Draw();

	if(!postProcessed && numPostProcesses > 0) {
		ApplyPostProcesses();
	}

	PresentSwapChain();
	ResetRenderTarget();
}

void GraphicsAPI::ApplyPostProcesses() {
	assert(!postProcessed && "attempted to run post processes twice in the same frame");
	postProcessed = true;

	uint32_t numPostProcesses = postProcesses.Size();
	for(uint32_t i = 0; i < numPostProcesses; i++) {
		RenderTarget* input = &postProcessTargets[i%2];
		RenderTarget* output = &postProcessTargets[1 - (i%2)];
		if(i == numPostProcesses - 1) {
			output = GetBackBuffer();
		}

		if(postProcesses[i]->IsActive()) {
			postProcesses[i]->Render(this, input, output);
		} else {
			// if the post process makes no changes, copying the pixels is fastest
			CopyTexture(input, output);
		}
	}

	ResetRenderTarget();
}

RenderTarget* GraphicsAPI::GetPostProcessHelper(uint8_t slot) {
	assert(slot >= 0 && slot < MAX_POST_PROCESS_HELPER_TEXTURES && "index was out of range");
	return &postProcessHelpers[slot];
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
	SetVertexShader(&app.assets.fullscreenVS);
	DrawVertices(3); // fullscreen triangle
}

void GraphicsAPI::SetVertexShader(const VertexShader* shader, void* constantInput, uint32_t inputBytes) {
	WriteBuffer(constantInput, inputBytes, shader->constants.data);
	SetConstants(ShaderStage::Vertex, &shader->constants, OBJECT_CONSTANT_REGISTER);
	SetVertexShader(shader);
}

void GraphicsAPI::SetGeometryShader(const GeometryShader* shader, void* constantInput, uint32_t inputBytes) {
	WriteBuffer(constantInput, inputBytes, shader->constants.data);
	SetConstants(ShaderStage::Geometry, &shader->constants, OBJECT_CONSTANT_REGISTER);
	SetGeometryShader(shader);
}

void GraphicsAPI::SetPixelShader(const PixelShader* shader, void* constantInput, uint32_t inputBytes) {
	WriteBuffer(constantInput, inputBytes, shader->constants.data);
	SetConstants(ShaderStage::Pixel, &shader->constants, OBJECT_CONSTANT_REGISTER);
	SetPixelShader(shader);
}
