#pragma once
#include "IPostProcess.h"
#include "PixelShader.h"
#include <memory>

class HSVPostProcess :
    public IPostProcess
{
public:
    // range: 0-1
    float contrast;
    float saturation;
    float brightness;

    HSVPostProcess();
    HSVPostProcess(float contrast, float saturation, float brightness);
    void Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) override;

private:
    std::shared_ptr<PixelShader> shader;
};

