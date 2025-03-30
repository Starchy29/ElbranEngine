#include "GraphicsAPI.h"
#include "Game.h"
#include <cassert>

GraphicsAPI::GraphicsAPI() {
	postProcessed = false;
	viewportDims = { 0, 0 };
	viewportOffset = { 0, 0 };
}

GraphicsAPI::~GraphicsAPI() {
	for(IPostProcess* pp : postProcesses) {
		delete pp;
	}
}

void GraphicsAPI::Render(Game* game) {
	postProcessed = false;
	ClearRenderTarget();
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
	assert(!postProcessed && "attempted to run post processes twice");
	postProcessed = true;

	for(int i = 0; i < postProcesses.size(); i++) {
		Texture2D* input = &postProcessTargets[i%2];
		Texture2D* output = &postProcessTargets[1 - (i%2)];
		if(i == postProcesses.size() - 1) {
			Texture2D backBuffer = GetBackBufferView();
			output = &backBuffer;
		}

		if(postProcesses[i]->IsActive()) {
			postProcesses[i]->Render(input, output);
		} else {
			CopyTexture(input, output); // if the post process makes no changes, copying the pixels is fastest
		}
	}

	ResetRenderTarget();
}

const Texture2D* GraphicsAPI::GetPostProcessHelper(int slot) {
	assert(slot >= 0 && slot < MAX_POST_PROCESS_HELPER_TEXTURES && "index was out of range");
	return &postProcessHelpers[slot];
}

Int2 GraphicsAPI::GetViewDimensions() const {
	return viewportDims;
}

Int2 GraphicsAPI::GetViewOffset() const {
	return viewportOffset;
}
