#pragma once
#include "IRenderer.h"
#include "Color.h"
#include "Sprite.h"

class StretchRenderer :
    public IRenderer
{
public:
    std::shared_ptr<Sprite> sprite;
    Color tint;
    Vector2 baseScale;
    Vector2 startUV;
    Vector2 endUV;
    bool flipX;
    bool flipY;

    StretchRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale, Vector2 startUV, Vector2 endUV);

    void Draw(Camera* camera, const Transform& transform) override;
    IRenderer* Clone() override;
};

