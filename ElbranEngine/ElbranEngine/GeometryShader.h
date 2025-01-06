#pragma once
#include "Shader.h"

class GeometryShader :
    public Shader
{
public:
    static void ClearShader();

    GeometryShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName);

    void SetArrayBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot) override;

private:
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> dxShader;

    void CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) override;
    void SetSpecificShader() override;
    void SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer) override;
};