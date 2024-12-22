#pragma once
#include "IRenderer.h"
#include "Color.h"
#include "Sprite.h"

class RepeatRenderer :
    public IRenderer
{
public:
    std::shared_ptr<Sprite> sprite;
    Color tint;
    Vector2 baseScale;
    bool flipX;
    bool flipY;

    RepeatRenderer(std::shared_ptr<Sprite> sprite, Vector2 baseScale);

    virtual void Draw(Camera* camera, const Transform& transform) override;
    virtual IRenderer* Clone() override;
};

