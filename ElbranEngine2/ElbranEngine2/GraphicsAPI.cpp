#include "GraphicsAPI.h"
// #include "Game.h"

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

Int2 GraphicsAPI::GetViewDimensions() const {
	return viewportDims;
}

Int2 GraphicsAPI::GetViewOffset() const {
	return viewportOffset;
}
