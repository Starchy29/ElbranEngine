#include "PostProcess.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "ShaderConstants.h"

void PostProcess::Render(const RenderTarget* input, RenderTarget* output, GraphicsAPI* graphics, const AssetContainer* assets) const {
	PostProcess otherStep;

	switch(type) {
	case Type::Blur: {
		const PixelShader* blurShader = &assets->blurPP;
		RenderTarget* midTarget = &graphics->postProcessHelpers[0];

		BlurPPConstants psInput = {};
		psInput.viewMin = graphics->viewportOffset;
		psInput.viewMax = psInput.viewMin + graphics->viewportDims;
		psInput.blurRadius = blurData.blurRadius;
		graphics->SetConstants(ShaderStage::Pixel, &blurShader->constants, 0);
		graphics->SetPixelShader(blurShader);

		// blur horizontal
		psInput.horizontal = true;
		graphics->WriteBuffer(&psInput, sizeof(BlurPPConstants), blurShader->constants.data);
		graphics->SetRenderTarget(midTarget, false);
		graphics->SetTexture(ShaderStage::Pixel, input, 0);
		graphics->DrawFullscreen(assets);

		// blur vertical
		psInput.horizontal = false;
		graphics->WriteBuffer(&psInput, sizeof(BlurPPConstants), blurShader->constants.data);
		graphics->SetRenderTarget(output, false);
		graphics->SetTexture(ShaderStage::Pixel, midTarget, 0);
		graphics->DrawFullscreen(assets);

		graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
	} break;

	case Type::HSV: {
		graphics->SetRenderTarget(output, false);

		ConSatValPPConstants psInput = {};
		psInput.contrast = hsvData.contrast;
		psInput.brightness = hsvData.brightness;
		psInput.saturation = hsvData.saturation;

		if(hsvData.contrast != 0) {
			// determine average brightness for contrast adjustment
			const ComputeShader* totalShader = &assets->brightnessSumCS;
			UInt2 viewDims = graphics->viewportDims;
			graphics->WriteBuffer(&viewDims, sizeof(UInt2), totalShader->constants.data);
			graphics->SetConstants(ShaderStage::Compute, &totalShader->constants, 0);
			graphics->SetTexture(ShaderStage::Compute, input, 0);

			uint32_t sums[4] = {};
			graphics->SetOutputBuffer(&graphics->totalBrightnessBuffer, 0, &sums);
			graphics->RunComputeShader(totalShader, viewDims.x, viewDims.y);
			graphics->ReadBuffer(&graphics->totalBrightnessBuffer, &sums);

			float totalBrightness = (sums[0] + sums[1] + sums[2] + sums[3]) / 100.0f; // shader multiplies by 100, cancel it out
			psInput.averageBrightness = totalBrightness / (viewDims.x * viewDims.y);
			graphics->SetTexture(ShaderStage::Compute, nullptr, 0); // unbind input texture
		}

		graphics->SetTexture(ShaderStage::Pixel, input, 0);
		graphics->SetPixelShader(&assets->conSatValPP, &psInput, sizeof(ConSatValPPConstants));

		graphics->DrawFullscreen(assets);
		graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
	} break;

	case Type::Bloom: {
		// extract bright pixels
		RenderTarget* brightPixels = &graphics->postProcessHelpers[1]; // blur shader uses slot 0
		graphics->SetRenderTarget(brightPixels, false);
		graphics->SetTexture(ShaderStage::Pixel, input, 0);
		graphics->SetPixelShader(&assets->bloomFilterPP, &bloomData.brightnessThreshold, 2 * sizeof(float));
		graphics->DrawFullscreen(assets);

		// blur bright pixels
		RenderTarget* blurredBrightness = &graphics->postProcessHelpers[2];
		otherStep.Blur(bloomData.blurRadius);
		otherStep.Render(brightPixels, blurredBrightness, graphics, assets);

		// combine blurred brightness with original
		graphics->SetRenderTarget(output, false);
		graphics->SetTexture(ShaderStage::Pixel, input, 0);
		graphics->SetTexture(ShaderStage::Pixel, blurredBrightness, 1);
		graphics->SetPixelShader(&assets->screenSumPP);
		graphics->DrawFullscreen(assets);

		graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
		graphics->SetTexture(ShaderStage::Pixel, nullptr, 1);
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
