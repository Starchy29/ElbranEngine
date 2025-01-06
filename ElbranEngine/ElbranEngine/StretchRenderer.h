#pragma once
#include "SpriteRenderer.h"
#include "Color.h"
#include "Sprite.h"

class StretchRenderer :
    public SpriteRenderer
{
public:
    Vector2 baseScale;
    Vector2 startUV;
    Vector2 endUV;

    StretchRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale, Vector2 startUV, Vector2 endUV);

    void Draw(Camera* camera, const Transform& transform) override;
    IBehavior* Clone() override;
};

