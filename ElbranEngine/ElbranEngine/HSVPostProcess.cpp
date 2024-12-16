#include "HSVPostProcess.h"
#include "Application.h"

HSVPostProcess::HSVPostProcess() {
	shader = APP->Assets()->conSatValPP;
	brightness = 0;
	contrast = 0;
	saturation = 0;
}

HSVPostProcess::HSVPostProcess(float contrast, float saturation, float brightness)
	: HSVPostProcess()
{
	this->contrast = contrast;
	this->saturation = saturation;
	this->brightness = brightness;
}

void HSVPostProcess::Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) {
	DXCore* graphics = APP->Graphics();
	graphics->GetContext()->OMSetRenderTargets(1, outputTexture.GetAddressOf(), nullptr);

	shader->SetConstantVariable("contrast", &contrast);
	shader->SetConstantVariable("saturation", &saturation);
	shader->SetConstantVariable("brightness", &brightness);
	shader->SetTexture(inputTexture, 0);
	shader->SetShader();

	graphics->DrawScreen();
}
