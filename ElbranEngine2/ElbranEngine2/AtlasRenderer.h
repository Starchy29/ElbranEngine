#pragma once
#include "IRenderer.h"
#include "Buffers.h"
#include "Common.h"

class AtlasRenderer :
    public IRenderer
{
public:
    const SpriteSheet* atlas;
    int row;
    int col;

    Color tint;
    bool lit;
    bool flipX;
    bool flipY;

    AtlasRenderer(const SpriteSheet* atlas);

    void Draw() override;
};

