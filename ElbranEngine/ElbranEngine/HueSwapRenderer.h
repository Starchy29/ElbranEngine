#pragma once
#include "SpriteRenderer.h"

class HueSwapRenderer :
    public SpriteRenderer
{
public:
    Color replaced;
    Color replacement;
    float sensitivity; // 0 - 3 range

    HueSwapRenderer(std::shared_ptr<Sprite> sprite);

    virtual void Draw(Camera* camera, const Transform& transform) override;
    virtual IRenderer* Clone() override;
};

