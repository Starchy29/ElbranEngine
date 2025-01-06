#pragma once
#include "SpriteRenderer.h"
#include "SpriteAtlas.h"

class AtlasRenderer :
    public SpriteRenderer
{
public:
    int row;
    int col;

    AtlasRenderer(std::shared_ptr<SpriteAtlas> sprite);

    void Draw(Camera* camera, const Transform& transform) override;
    IBehavior* Clone() override;
};

