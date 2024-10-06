#pragma once
#include "Shader.h"

class PixelShader :
    public Shader
{
public:
    PixelShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName);

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> dxShader;

    void SetSpecificShader() override;
    void CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) override;
    void SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer);
};

