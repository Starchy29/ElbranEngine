#pragma once
#include "IPostProcess.h"
#include "BlurPostProcess.h"

class BloomPostProcess :
    public IPostProcess
{
public:
    float threshold;

    BloomPostProcess(float threshold, int blurRadius);
    ~BloomPostProcess();

    void SetBlurRadius(int radius);
    void Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) override;

private:
    BlurPostProcess* blurStep;
    std::shared_ptr<PixelShader> filter;
    std::shared_ptr<PixelShader> combiner;
};

