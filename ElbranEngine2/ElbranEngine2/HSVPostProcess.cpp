#include "HSVPostProcess.h"
#include "GraphicsAPI.h"
#include "ShaderConstants.h"
#include "Application.h"
#include "AssetContainer.h"

HSVPostProcess::HSVPostProcess() {
	contrast = 0.f;
	saturation = 0.f;
	brightness = 0.f;

	ppShader = &app->assets->conSatValPP;
	totalShader = &app->assets->brightnessSumCS;
	totalBrightnessBuffer = app->graphics->CreateOutputBuffer(ShaderDataType::UInt, 4); // should match BrightnessSumCS.hlsl
}

HSVPostProcess::~HSVPostProcess() {
	totalBrightnessBuffer.Release();
}

void HSVPostProcess::Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) {
	graphics->SetRenderTarget(output, false);

	ConSatValPPConstants psInput = {};
	psInput.contrast = contrast;
	psInput.brightness = brightness;
	psInput.saturation = saturation;

	if(contrast != 0) {
		// determine average brightness for contrast adjustment
		Int2 viewDims = graphics->GetViewDimensions();
		graphics->WriteBuffer(&viewDims, sizeof(Int2), totalShader->constants.data);
		graphics->SetConstants(ShaderStage::Compute, &totalShader->constants, 0);
		graphics->SetTexture(ShaderStage::Compute, input, 0);
		
		unsigned int sums[4] = {};
		graphics->SetOutputBuffer(&totalBrightnessBuffer, 0, &sums);
		graphics->RunComputeShader(totalShader, viewDims.x, viewDims.y);
		graphics->ReadBuffer(&totalBrightnessBuffer, &sums);

		float totalBrightness = (sums[0] + sums[1] + sums[2] + sums[3]) / 100.0f; // shader multiplies by 100, cancel it out
		psInput.averageBrightness = totalBrightness / (viewDims.x * viewDims.y);

		// unbind input texture so it can bind to pixel shader next
		Texture2D nullTex = {};
		graphics->SetTexture(ShaderStage::Compute, &nullTex, 0);
	}

	graphics->WriteBuffer(&psInput, sizeof(ConSatValPPConstants), ppShader->constants.data);
	graphics->SetConstants(ShaderStage::Pixel, &ppShader->constants, 0);
	graphics->SetTexture(ShaderStage::Pixel, input, 0);
	graphics->SetPixelShader(ppShader);

	graphics->DrawFullscreen();
}

bool HSVPostProcess::IsActive() const {
	return contrast != 0.f || saturation != 0.f || brightness != 0.f;
}
