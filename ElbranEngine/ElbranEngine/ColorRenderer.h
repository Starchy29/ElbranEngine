#pragma once
#include "IRenderer.h"
#include "Color.h"

class ColorRenderer :
    public IRenderer
{
public:
    Color color;

    ColorRenderer(Color color, bool circle = false);

    void Draw(Camera* camera, const Transform& transform) override;
    IRenderer* Clone() override;
};

