#include "BrightConPostProc.h"
#include "Application.h"

BrightConPostProc::BrightConPostProc() {
	shader = APP->Assets()->brightnessContrastPP;
	brightness = 0;
	contrast = 0;
}

void BrightConPostProc::Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) {
	DXCore* graphics = APP->Graphics();
	graphics->GetContext()->OMSetRenderTargets(1, outputTexture.GetAddressOf(), nullptr);

	shader->SetTexture(inputTexture, 0);
	shader->SetShader();

	graphics->DrawScreen();
}
