#pragma once
#include "Shader.h"

class PixelShader :
    public Shader
{
public:
    PixelShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName);

    void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView, int slot = 0);
    void SetSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, int slot = 0);
    void SetArrayBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot) override;

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> dxShader;

    void SetSpecificShader() override;
    void CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) override;
    void SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer);
};

