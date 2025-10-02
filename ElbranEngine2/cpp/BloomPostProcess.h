#pragma once
#include "IPostProcess.h"
#include "BlurPostProcess.h"

class BloomPostProcess :
    public IPostProcess
{
public:
    float threshold; // 0-1
    BlurPostProcess blurStep;

    BloomPostProcess() {}
    void Initialize();

    void Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output) override;
    bool IsActive() const override;
};

