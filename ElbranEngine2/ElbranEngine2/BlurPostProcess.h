#pragma once
#include "IPostProcess.h"
#include "Shaders.h"

class BlurPostProcess :
    public IPostProcess
{
public:
    int blurRadius;

    BlurPostProcess();

    void Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) override;
    bool IsActive() const override;

private:
    PixelShader* blurShader;
};

