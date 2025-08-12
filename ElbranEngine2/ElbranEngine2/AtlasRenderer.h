#pragma once
#include "IRenderer.h"
#include "Buffers.h"
#include "Common.h"

class AtlasRenderer :
    public IRenderer
{
public:
    const SpriteSheet* atlas;
    uint8_t row;
    uint8_t col;

    Color tint;
    bool lit;
    bool flipX;
    bool flipY;

    AtlasRenderer(const SpriteSheet* atlas);

    void Draw() override;
};

