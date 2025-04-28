#include "BlurPostProcess.h"
#include "GraphicsAPI.h"
#include "ShaderConstants.h"
#include "Application.h"
#include "AssetContainer.h"

BlurPostProcess::BlurPostProcess() {
	blurRadius = 0;
	blurShader = &app->assets->blurPP;
}

void BlurPostProcess::Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) {
	RenderTarget* midTarget = graphics->GetPostProcessHelper(0);

	BlurPPConstants psInput = {};
	psInput.viewMin = graphics->GetViewOffset();
	psInput.viewMax = psInput.viewMin + graphics->GetViewDimensions();
	psInput.blurRadius = blurRadius;
	graphics->SetConstants(ShaderStage::Pixel, &blurShader->constants, 0);
	graphics->SetPixelShader(blurShader);

	// blur horizontal
	psInput.horizontal = true;
	graphics->WriteBuffer(&psInput, sizeof(BlurPPConstants), blurShader->constants.data);
	graphics->SetRenderTarget(midTarget, false);
	graphics->SetTexture(ShaderStage::Pixel, input, 0);
	graphics->DrawFullscreen();

	// blur vertical
	psInput.horizontal = false;
	graphics->WriteBuffer(&psInput, sizeof(BlurPPConstants), blurShader->constants.data);
	graphics->SetRenderTarget(output, false);
	graphics->SetTexture(ShaderStage::Pixel, midTarget, 0);
	graphics->DrawFullscreen();

	graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
}

bool BlurPostProcess::IsActive() const {
	return blurRadius > 0;
}
