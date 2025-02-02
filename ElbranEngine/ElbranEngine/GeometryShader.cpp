#include "GeometryShader.h"
#include "Application.h"
#include "DXCore.h"

void GeometryShader::ClearShader() {
	APP->Graphics()->GetContext()->GSSetShader(NULL, 0, 0);
}

GeometryShader::GeometryShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName)
	: Shader(device, context)
{
	LoadShader(fileName);
}

void GeometryShader::SetArrayBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot) {
	dxContext->GSSetShaderResources(slot, 1, view.GetAddressOf());
}

void GeometryShader::CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) {
	HRESULT result = dxDevice->CreateGeometryShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		dxShader.GetAddressOf()
	);

	if(result != S_OK) {
		throw std::exception("failed to create vertex shader");
	}
}

void GeometryShader::SetSpecificShader() {
	dxContext->GSSetShader(dxShader.Get(), 0, 0);
}

void GeometryShader::SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer) {
	dxContext->GSSetConstantBuffers(slot, 1, cBuffer.GetAddressOf());
}
