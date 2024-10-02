#pragma once
#include "Shader.h"

class VertexShader :
    public Shader
{
public:
    VertexShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName);
    void SetShader() override;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> dxShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

    void CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) override;
};

