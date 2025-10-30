#include "BloomPostProcess.h"
#include "GraphicsAPI.h"
#include "Application.h"

void BloomPostProcess::Initialize() {
    threshold = 1.0f;
    blurStep.blurRadius = 50;
}

void BloomPostProcess::Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) {
    // extract bright pixels
    RenderTarget* brightPixels = graphics->GetPostProcessHelper(1); // blur shader uses slot 0
    graphics->SetRenderTarget(brightPixels, false);
    graphics->SetTexture(ShaderStage::Pixel, input, 0);
    graphics->SetPixelShader(&app->assets.bloomFilterPP, &threshold, sizeof(float));
    graphics->DrawFullscreen();

    // blur bright pixels
    RenderTarget* blurredBrightness = graphics->GetPostProcessHelper(2);
    blurStep.Render(graphics, brightPixels, blurredBrightness);

    // combine blurred brightness with original
    graphics->SetRenderTarget(output, false);
    graphics->SetTexture(ShaderStage::Pixel, input, 0);
    graphics->SetTexture(ShaderStage::Pixel, blurredBrightness, 1);
    graphics->SetPixelShader(&app->assets.screenSumPP);
    graphics->DrawFullscreen();

    graphics->SetTexture(ShaderStage::Pixel, nullptr, 0);
    graphics->SetTexture(ShaderStage::Pixel, nullptr, 1);
}

bool BloomPostProcess::IsActive() const {
    return threshold < 1.f;
}
