#pragma once
#include "Shader.h"

class ComputeShader
	: Shader
{
public:
	ComputeShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName);

	void SetSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, int slot = 0);
	void SetReadOnlyBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot);
	void SetReadWriteBuffer(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> view, int slot);
	void Dispatch(int xSize, int ySize, int zSize);

private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> dxShader;
	UINT xGroupSize;
	UINT yGroupSize;
	UINT zGroupSize;

	using Shader::SetShader;

	void SetSpecificShader() override;
	void CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) override;
	void SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer) override;
};