#pragma once
#include "IPostProcess.h"
#include "Shaders.h"

class HSVPostProcess :
    public IPostProcess
{
public:
    // range: -1 to 1
    float contrast;
    float saturation;
    float brightness;

    HSVPostProcess();
    ~HSVPostProcess();
    void Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) override;
    bool IsActive() const override;

private:
    PixelShader* ppShader;
    ComputeShader* totalShader;
    OutputBuffer totalBrightnessBuffer;
};

