#include "BloomPostProcess.h"
#include "Application.h"

BloomPostProcess::BloomPostProcess(float threshold, int blurRadius) {
	this->threshold = threshold;
	blurStep = new BlurPostProcess(blurRadius);
	filter = APP->Assets()->bloomFilterPP;
	combiner = APP->Assets()->screenSumPP;
}

BloomPostProcess::~BloomPostProcess() {
	delete blurStep;
}

void BloomPostProcess::SetBlurRadius(int radius) {
	blurStep->blurRadius = radius;
}

void BloomPostProcess::Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) {
	DXCore* directX = APP->Graphics();

	// extract bright pixels
	PostProcessTexture* blurInput = directX->GetPostProcessTexture(1); // blur shader uses slot 0
	directX->GetContext()->OMSetRenderTargets(1, blurInput->GetRenderTarget().GetAddressOf(), nullptr);
	filter->SetConstantVariable("threshold", &threshold);
	filter->SetTexture(inputTexture, 0);
	filter->SetShader();
	directX->DrawScreen();

	// blur bright pixels
	PostProcessTexture* blurOutput = directX->GetPostProcessTexture(2);
	blurStep->Render(blurInput->GetShaderResource(), blurOutput->GetRenderTarget());

	// combine blurred brightness with original
	directX->GetContext()->OMSetRenderTargets(1, outputTexture.GetAddressOf(), nullptr);
	combiner->SetTexture(inputTexture, 0);
	combiner->SetTexture(blurOutput->GetShaderResource(), 1);
	combiner->SetShader();
	directX->DrawScreen();
}

bool BloomPostProcess::IsActive() {
	return threshold <= 1.0f;
}
