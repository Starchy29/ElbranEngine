#include "GraphicsAPI.h"
#include "Game.h"
#include <cassert>

GraphicsAPI::GraphicsAPI() {
	postProcessed = false;
	viewportDims = { 0, 0 };
	viewportOffset = { 0, 0 };
}

GraphicsAPI::~GraphicsAPI() {

}

void GraphicsAPI::Render(Game* game) {
	postProcessed = false;
	ClearRenderTarget();
	ClearDepthStencil();

	//if(postProcesses.size() > 0) {
		// SetRenderTarget(ppTex1);
	//}

	//game->Draw();

	//if(!postProcessed && postProcesses.size() > 0) {
		//ApplyPostProcesses();
	//}

	PresentSwapChain();
}

void GraphicsAPI::ApplyPostProcesses() {
	assert(!postProcessed && "attempted to run post processes twice");

	postProcessed = true;

	/*for (int i = 0; i < postProcesses.size(); i++) {
		PostProcessTexture* input;
		PostProcessTexture* output;
		if(i % 2 == 0) {
			input = &ppTex1;
			output = &ppTex2;
		} else {
			input = &ppTex2;
			output = &ppTex1;
		}

		if(postProcesses[i]->IsActive()) {
			postProcesses[i]->Render(input->GetShaderResource(), i == postProcesses.size() - 1 ? backBufferView : output->GetRenderTarget());
		} else {
			// if the post process makes no changes, copying the pixels is faster
			ID3D11Resource* outputResource;
			if(i == postProcesses.size() - 1) {
				backBufferView->GetResource(&outputResource);
			} else {
				outputResource = output->GetTexture().Get();
			}
			context->CopyResource(outputResource, input->GetTexture().Get());
			outputResource->Release();
		}
	}*/

	//context->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());
}

Int2 GraphicsAPI::GetViewDimensions() const {
	return viewportDims;
}

Int2 GraphicsAPI::GetViewOffset() const {
	return viewportOffset;
}
