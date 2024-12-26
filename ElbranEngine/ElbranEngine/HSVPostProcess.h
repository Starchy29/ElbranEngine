#pragma once
#include "IPostProcess.h"
#include "PixelShader.h"
#include "ComputeShader.h"
#include <memory>

class HSVPostProcess :
    public IPostProcess
{
public:
    // range: -1 to 1
    float contrast;
    float saturation;
    float brightness;

    HSVPostProcess();
    HSVPostProcess(float contrast, float saturation, float brightness);
    void Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) override;

private:
    std::shared_ptr<PixelShader> shader;
    std::shared_ptr<ComputeShader> brightComputer;
    RWArrayBuffer totalBrightnessBuffer;
};

