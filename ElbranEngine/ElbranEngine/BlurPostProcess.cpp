#include "BlurPostProcess.h"
#include "Application.h"

BlurPostProcess::BlurPostProcess(int blurRadius) {
	this->blurRadius = blurRadius;
	shader = APP->Assets()->blurPP;
}

void BlurPostProcess::Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) {
	DXCore* directX = APP->Graphics();
	PostProcessTexture* midTex = directX->GetPostProcessTexture(0);

	DirectX::XMINT2 viewMin = directX->GetViewOffset();
	DirectX::XMINT2 viewDims = directX->GetViewDimensions();;
	DirectX::XMINT2 viewMax = DirectX::XMINT2(viewMin.x + viewDims.x, viewMin.y + viewDims.y);
	shader->SetConstantVariable("viewMin", &viewMin);
	shader->SetConstantVariable("viewMax", &viewMax);
	shader->SetConstantVariable("blurRadius", &blurRadius);

	// blur horizontal
	directX->GetContext()->OMSetRenderTargets(1, midTex->GetRenderTarget().GetAddressOf(), nullptr);
	shader->SetTexture(inputTexture, 0);
	shader->SetBool("horizontal", true);
	shader->SetShader();
	directX->DrawScreen();

	// blur vertical
	directX->GetContext()->OMSetRenderTargets(1, outputTexture.GetAddressOf(), nullptr);
	shader->SetTexture(midTex->GetShaderResource(), 0);
	shader->SetBool("horizontal", false);
	shader->SetShader();
	directX->DrawScreen();
}