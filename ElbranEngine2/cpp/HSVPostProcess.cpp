#include "HSVPostProcess.h"
#include "GraphicsAPI.h"
#include "ShaderConstants.h"
#include "Application.h"

void HSVPostProcess::Initialize() {
	contrast = 0.f;
	saturation = 0.f;
	brightness = 0.f;

	totalBrightnessBuffer = app->graphics->CreateOutputBuffer(ShaderDataType::UInt, 4); // should match BrightnessSumCS.hlsl
}

void HSVPostProcess::Release() {
	app->graphics->ReleaseOuputBuffer(&totalBrightnessBuffer);
}

void HSVPostProcess::Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) {
	graphics->SetRenderTarget(output, false);

	ConSatValPPConstants psInput = {};
	psInput.contrast = contrast;
	psInput.brightness = brightness;
	psInput.saturation = saturation;

	if(contrast != 0) {
		// determine average brightness for contrast adjustment
		ComputeShader* totalShader = &app->assets.brightnessSumCS;
		UInt2 viewDims = graphics->GetViewDimensions();
		graphics->WriteBuffer(&viewDims, sizeof(UInt2), totalShader->constants.data);
		graphics->SetConstants(ShaderStage::Compute, &totalShader->constants, 0);
		graphics->SetTexture(ShaderStage::Compute, input, 0);
		
		uint32_t sums[4] = {};
		graphics->SetOutputBuffer(&totalBrightnessBuffer, 0, &sums);
		graphics->RunComputeShader(totalShader, viewDims.x, viewDims.y);
		graphics->ReadBuffer(&totalBrightnessBuffer, &sums);

		float totalBrightness = (sums[0] + sums[1] + sums[2] + sums[3]) / 100.0f; // shader multiplies by 100, cancel it out
		psInput.averageBrightness = totalBrightness / (viewDims.x * viewDims.y);
		graphics->SetTexture(ShaderStage::Compute, nullptr, 0); // unbind input texture
	}

	graphics->SetTexture(ShaderStage::Pixel, input, 0);
	graphics->SetPixelShader(&app->assets.conSatValPP, &psInput, sizeof(ConSatValPPConstants));

	graphics->DrawFullscreen();
	graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
}

bool HSVPostProcess::IsActive() const {
	return contrast != 0.f || saturation != 0.f || brightness != 0.f;
}
