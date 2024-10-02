#include "Shader.h"
#include "DXGame.h"

Shader::Shader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) {
	this->device = device;
	this->context = context;
}

void Shader::LoadShader(std::wstring fileName) {
	std::wstring fileString = GameInstance->exePath + fileName;
	LPCWSTR filePath = fileString.c_str();
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	HRESULT hr = D3DReadFileToBlob(filePath, shaderBlob.GetAddressOf());
	if(hr != S_OK) {
		throw std::invalid_argument("failed to read shader file");
	}

	CreateShader(shaderBlob);
}
