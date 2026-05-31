#include "PostProcess.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

void PostProcess::Render(const RenderTarget* input, RenderTarget* output, DrawComponents app) const {
	PostProcess otherStep;

	switch(type) {
	case Type::Blur: {
		const PixelShader* blurShader = &app.assets->blurPP;
		RenderTarget* midTarget = &app.graphics->postProcessHelpers[0];

		BlurPPConstants psInput = {};
		psInput.viewMin = app.graphics->viewportOffset;
		psInput.viewMax = psInput.viewMin + app.graphics->viewportDims;
		psInput.blurRadius = blurData.blurRadius;
		app.graphics->SetConstants(ShaderStage::Pixel, blurShader->constants, 0);
		app.graphics->SetPixelShader(blurShader);

		// blur horizontal
		psInput.horizontal = true;
		app.graphics->WriteBuffer(&psInput, sizeof(BlurPPConstants), blurShader->constants);
		app.graphics->SetRenderTarget(midTarget, false);
		app.graphics->SetTexture(ShaderStage::Pixel, &input->texture, 0);
		app.graphics->DrawFullscreen(app.assets);

		// blur vertical
		psInput.horizontal = false;
		app.graphics->WriteBuffer(&psInput, sizeof(BlurPPConstants), blurShader->constants);
		app.graphics->SetRenderTarget(output, false);
		app.graphics->SetTexture(ShaderStage::Pixel, &midTarget->texture, 0);
		app.graphics->DrawFullscreen(app.assets);

		app.graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
	} break;

	case Type::HSV: {
		app.graphics->SetRenderTarget(output, false);

		ConSatValPPConstants psInput = {};
		psInput.contrast = hsvData.contrast;
		psInput.brightness = hsvData.brightness;
		psInput.saturation = hsvData.saturation;

		if(hsvData.contrast != 0) {
			// determine average brightness for contrast adjustment
			const ComputeShader* totalShader = &app.assets->brightnessSumCS;
			UInt2 viewDims = app.graphics->viewportDims;
			app.graphics->WriteBuffer(&viewDims, sizeof(UInt2), totalShader->constants);
			app.graphics->SetConstants(ShaderStage::Compute, totalShader->constants, 0);
			app.graphics->SetTexture(ShaderStage::Compute, &input->texture, 0);

			uint32_t sums[4] = {};
			app.graphics->SetOutputBuffer(&app.graphics->totalBrightnessBuffer, 0, &sums);
			app.graphics->RunComputeShader(totalShader, viewDims.x, viewDims.y);
			app.graphics->ReadBuffer(&app.graphics->totalBrightnessBuffer, &sums);

			float totalBrightness = (sums[0] + sums[1] + sums[2] + sums[3]) / 100.0f; // shader multiplies by 100, cancel it out
			psInput.averageBrightness = totalBrightness / (viewDims.x * viewDims.y);
			app.graphics->SetTexture(ShaderStage::Compute, nullptr, 0); // unbind input texture
		}

		app.graphics->SetTexture(ShaderStage::Pixel, &input->texture, 0);
		app.graphics->SetPixelShader(&app.assets->conSatValPP, &psInput, sizeof(ConSatValPPConstants));

		app.graphics->DrawFullscreen(app.assets);
		app.graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
	} break;

	case Type::Bloom: {
		// extract bright pixels
		RenderTarget* brightPixels = &app.graphics->postProcessHelpers[1]; // blur shader uses slot 0
		app.graphics->SetRenderTarget(brightPixels, false);
		app.graphics->SetTexture(ShaderStage::Pixel, &input->texture, 0);
		app.graphics->SetPixelShader(&app.assets->bloomFilterPP, &bloomData.brightnessThreshold, 2 * sizeof(float));
		app.graphics->DrawFullscreen(app.assets);

		// blur bright pixels
		RenderTarget* blurredBrightness = &app.graphics->postProcessHelpers[2];
		otherStep.Blur(bloomData.blurRadius);
		otherStep.Render(brightPixels, blurredBrightness, app);

		// combine blurred brightness with original
		app.graphics->SetRenderTarget(output, false);
		app.graphics->SetTexture(ShaderStage::Pixel, &input->texture, 0);
		app.graphics->SetTexture(ShaderStage::Pixel, &blurredBrightness->texture, 1);
		app.graphics->SetPixelShader(&app.assets->screenSumPP);
		app.graphics->DrawFullscreen(app.assets);

		app.graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
		app.graphics->SetTexture(ShaderStage::Pixel, nullptr, 1);
	} break;
	}
}

bool PostProcess::IsActive() const {
	switch(type) {
	case Type::Blur: return blurData.blurRadius > 0;
	case Type::HSV: return hsvData.contrast != 0.f || hsvData.saturation != 0.f || hsvData.brightness != 0.f;
	case Type::Bloom: return bloomData.blurRadius > 0 && bloomData.brightnessThreshold < 1.0f;
	}

	return false;
}

void PostProcess::Blur(uint16_t blurRadius) {
	type = Type::Blur;
	blurData.blurRadius = blurRadius;
}

void PostProcess::HSV(float contrast, float saturation, float brightness) {
	type = Type::HSV;
	hsvData.contrast = contrast;
	hsvData.saturation = saturation;
	hsvData.brightness = brightness;
}

void PostProcess::Bloom(uint16_t blurRadius, float brightnessThreshold, float sensitivity) {
	type = Type::Bloom;
	bloomData.blurRadius = blurRadius;
	bloomData.brightnessThreshold = brightnessThreshold;
	bloomData.thresholdSensitivity = sensitivity;
}
