#pragma once
#include "SpriteRenderer.h"

class HueSwapRenderer :
    public SpriteRenderer
{
public:
    Color oldHue;
    Color newHue;
    float sensitivity; // 0 - 1 range

    HueSwapRenderer(std::shared_ptr<Sprite> sprite);

    virtual void Draw(Camera* camera, const Transform& transform) override;
    virtual IRenderer* Clone() override;
};

