#pragma once
#include "IPostProcess.h"
#include <memory>
#include "PixelShader.h"

class BlurPostProcess :
    public IPostProcess
{
public:
    int blurRadius;

    BlurPostProcess(int blurRadius);

    void Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) override;

private:
    std::shared_ptr<PixelShader> shader;
};

