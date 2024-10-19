#include "PixelShader.h"

PixelShader::PixelShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName)
	: Shader(device, context)
{
	LoadShader(fileName);
}

void PixelShader::SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView, int slot) {
	dxContext->PSSetShaderResources(slot, 1, resourceView.GetAddressOf());
}

void PixelShader::SetSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, int slot) {
	dxContext->PSSetSamplers(slot, 1, samplerState.GetAddressOf());
}

void PixelShader::SetSpecificShader() {
	dxContext->PSSetShader(dxShader.Get(), 0, 0);
}

void PixelShader::CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) {
	HRESULT result = dxDevice->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		dxShader.GetAddressOf()
	);

	if(result != S_OK) {
		throw std::exception("failed to create pixel shader");
	}
}

void PixelShader::SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer) {
	dxContext->PSSetConstantBuffers(slot, 1, cBuffer.GetAddressOf());
}
