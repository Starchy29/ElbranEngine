#include "BloomPostProcess.h"
#include "GraphicsAPI.h"
#include "Application.h"
#include "AssetContainer.h"

BloomPostProcess::BloomPostProcess() {
    threshold = 1.0f;
    blurStep = BlurPostProcess();
    blurStep.blurRadius = 30;

    filterShader = &app->assets->bloomFilterPP;
    combineShader = &app->assets->screenSumPP;

}

void BloomPostProcess::Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) {
    // extract bright pixels
    RenderTarget* brightPixels = graphics->GetPostProcessHelper(1); // blur shader uses slot 0
    graphics->SetRenderTarget(brightPixels, false);
    graphics->SetTexture(ShaderStage::Pixel, input, 0);
    graphics->SetPixelShader(filterShader, &threshold, sizeof(float));
    graphics->DrawFullscreen();

    // blur bright pixels
    RenderTarget* blurredBrightness = graphics->GetPostProcessHelper(2);
    blurStep.Render(graphics, brightPixels, blurredBrightness);

    // combine blurred brightness with original
    graphics->SetRenderTarget(output, false);
    graphics->SetTexture(ShaderStage::Pixel, input, 0);
    graphics->SetTexture(ShaderStage::Pixel, blurredBrightness, 1);
    graphics->SetPixelShader(combineShader);
    graphics->DrawFullscreen();

    graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
    graphics->SetTexture(ShaderStage::Pixel, nullptr, 1);
}

bool BloomPostProcess::IsActive() const {
    return threshold < 1.f;
}
