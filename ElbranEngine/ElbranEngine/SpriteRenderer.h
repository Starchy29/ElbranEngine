#pragma once
#include "IRenderer.h"
#include "Color.h"
#include "Sprite.h"

class SpriteRenderer :
    public IRenderer
{
public:
    std::shared_ptr<Sprite> sprite;
    Color tint;
    bool flipX;
    bool flipY;

    SpriteRenderer(std::shared_ptr<Sprite> sprite);

    void Draw(Camera* camera, const Transform& transform) override;
    IRenderer* Clone() override;
};
