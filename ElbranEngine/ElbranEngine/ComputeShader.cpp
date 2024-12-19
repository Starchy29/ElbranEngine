#include "ComputeShader.h"

ComputeShader::ComputeShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName) 
	: Shader(device, context)
{
	LoadShader(fileName);
}

void ComputeShader::SetReadOnlyBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot) {
	dxContext->CSSetShaderResources(slot, 1, view.GetAddressOf());
}

void ComputeShader::SetReadWriteBuffer(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> view, int slot) {
	dxContext->CSSetUnorderedAccessViews(slot, 1, view.GetAddressOf(), nullptr);
}

void ComputeShader::SetSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, int slot) {
	dxContext->CSSetSamplers(slot, 1, samplerState.GetAddressOf());
}

void ComputeShader::Dispatch(int xSize, int ySize, int zSize) {
	SetShader();
	dxContext->Dispatch(ceil((double)xSize / xGroupSize), ceil((double)ySize / yGroupSize), ceil((double)zSize / zGroupSize));
}

void ComputeShader::SetSpecificShader() {
	dxContext->CSSetShader(dxShader.Get(), 0, 0);
}

void ComputeShader::CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) {
	HRESULT result = dxDevice->CreateComputeShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		dxShader.GetAddressOf()
	);

	if(result != S_OK) {
		throw std::exception("failed to create compute shader");
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)reflection.GetAddressOf()
	);

	// determine  the thread group sizes
	reflection->GetThreadGroupSize(&xGroupSize, &yGroupSize, &zGroupSize);
}

void ComputeShader::SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer) {
	dxContext->CSSetConstantBuffers(slot, 1, cBuffer.GetAddressOf());
}
