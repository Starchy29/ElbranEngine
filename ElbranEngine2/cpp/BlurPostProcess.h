#pragma once
#include "IPostProcess.h"
#include "Shaders.h"

class BlurPostProcess :
    public IPostProcess
{
public:
    uint16_t blurRadius;

    BlurPostProcess() {}

    void Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) override;
    bool IsActive() const override;
};

