#include "GraphicsAPI.h"
#include "Game.h"
#include "Application.h"
#include "AssetContainer.h"
#include <cassert>

GraphicsAPI::GraphicsAPI() {
	postProcessed = false;
	viewportDims = { 0, 0 };
	viewportOffset = { 0, 0 };
	viewAspectRatio = 0.f;

	postProcessTargets[0] = {};
	postProcessTargets[1] = {};
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		postProcessHelpers[i] = {};
	}
}

GraphicsAPI::~GraphicsAPI() {
	for(IPostProcess* pp : postProcesses) {
		delete pp;
	}
	postProcessTargets[0].Release();
	postProcessTargets[1].Release();
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		postProcessHelpers[i].Release();
	}
}

void GraphicsAPI::Render(Game* game) {
	postProcessed = false;
	ClearBackBuffer();
	ClearDepthStencil();

	if(postProcesses.size() > 0) {
		SetRenderTarget(&postProcessTargets[0]);
	}

	//game->Draw();

	if(!postProcessed && postProcesses.size() > 0) {
		ApplyPostProcesses();
	}

	PresentSwapChain();
	ResetRenderTarget();
}

void GraphicsAPI::ApplyPostProcesses() {
	assert(!postProcessed && "attempted to run post processes twice in the same frame");
	postProcessed = true;

	for(int i = 0; i < postProcesses.size(); i++) {
		RenderTarget* input = &postProcessTargets[i%2];
		RenderTarget* output = &postProcessTargets[1 - (i%2)];
		if(i == postProcesses.size() - 1) {
			RenderTarget backBuffer = GetBackBufferView();
			output = &backBuffer;
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

const RenderTarget* GraphicsAPI::GetPostProcessHelper(int slot) {
	assert(slot >= 0 && slot < MAX_POST_PROCESS_HELPER_TEXTURES && "index was out of range");
	return &postProcessHelpers[slot];
}

Int2 GraphicsAPI::GetViewDimensions() const {
	return viewportDims;
}

Int2 GraphicsAPI::GetViewOffset() const {
	return viewportOffset;
}

float GraphicsAPI::GetViewAspectRatio() const {
	return viewAspectRatio;
}

void GraphicsAPI::DrawFullscreen() {
	SetVertexShader(&app->assets->fullscreenVS);
	DrawVertices(3); // fullscreen triangle
}
