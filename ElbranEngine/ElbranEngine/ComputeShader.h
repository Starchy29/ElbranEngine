#pragma once
#include "Shader.h"

// represents a RWBuffer<> or RWStructuredBuffer<>
struct RWArrayBuffer {
	UINT elements;
	UINT elementSize;
	Microsoft::WRL::ComPtr<ID3D11Buffer> gpuBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cpuBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> view;
};

struct ReadWriteTexture {
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
};

class ComputeShader
	: public Shader
{
public:
	ComputeShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName);
	~ComputeShader();

	static ReadWriteTexture CreateReadWriteTexture(UINT width, UINT height);
	static RWArrayBuffer CreateReadWriteBuffer(UINT elements, UINT elementBytes, ShaderDataType dataType);
	static void WriteBuffer(void* data, RWArrayBuffer* buffer);
	static void ReadBuffer(RWArrayBuffer* buffer, void* destination);

	void SetSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, int slot = 0);
	void SetArrayBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot) override;
	void SetReadWriteBuffer(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> view, int slot); // for RWTextures, RWBuffers, and RWStructuredBuffers

	void Dispatch(int xSize, int ySize, int zSize = 1);

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