#include "HSVPostProcess.h"
#include "Application.h"
#include "AssetManager.h"
#include "DXCore.h"

HSVPostProcess::HSVPostProcess() {
	shader = APP->Assets()->conSatValPP;
	brightComputer = APP->Assets()->brightnessSumCS;
	brightness = 0;
	contrast = 0;
	saturation = 0;

	totalBrightnessBuffer = ComputeShader::CreateReadWriteBuffer(4, sizeof(UINT), ShaderDataType::UInt);
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

	DirectX::XMUINT2 viewDims = graphics->GetViewDimensions();

	if (contrast != 0) {
		brightComputer->SetArrayBuffer(inputTexture, 0);
		brightComputer->SetConstantVariable("screenDims", &viewDims);

		UINT brightSums[4] = { 0, 0, 0, 0};
		ComputeShader::WriteBuffer(&brightSums, &totalBrightnessBuffer);
		brightComputer->SetReadWriteBuffer(totalBrightnessBuffer.view, 0);
		brightComputer->Dispatch(viewDims.x, viewDims.y);
		ComputeShader::ReadBuffer(&totalBrightnessBuffer, &brightSums);

		float totalBrightness = (brightSums[0] + brightSums[1] + brightSums[2] + brightSums[3]) / 100.f; // BrightnessSumCS.hlsl multiplies brightness by 100
		float average = totalBrightness / (viewDims.x * viewDims.y);
		shader->SetConstantVariable("averageBrightness", &average);

		ID3D11ShaderResourceView* nullSRV = nullptr;
		graphics->GetContext()->CSSetShaderResources(0, 1, &nullSRV);
	}

	shader->SetConstantVariable("contrast", &contrast);
	shader->SetConstantVariable("saturation", &saturation);
	shader->SetConstantVariable("brightness", &brightness);
	shader->SetTexture(inputTexture, 0);
	shader->SetShader();

	graphics->DrawScreen();
}

bool HSVPostProcess::IsActive() {
	return contrast != 0.f || saturation != 0.f || brightness != 0.f;
}
