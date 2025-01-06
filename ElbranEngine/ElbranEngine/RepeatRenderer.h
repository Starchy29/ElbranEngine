#pragma once
#include "SpriteRenderer.h"
#include "Color.h"
#include "Sprite.h"

class RepeatRenderer :
    public SpriteRenderer
{
public:
    Vector2 baseScale;

    RepeatRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale);

    virtual void Draw(Camera* camera, const Transform& transform) override;
    virtual IBehavior* Clone() override;
};

