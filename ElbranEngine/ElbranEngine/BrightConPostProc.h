#pragma once
#include "IPostProcess.h"
#include "PixelShader.h"
#include <memory>

class BrightConPostProc :
    public IPostProcess
{
public:
    float brightness;
    float contrast;

    BrightConPostProc();
    void Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) override;

private:
    std::shared_ptr<PixelShader> shader;
};

