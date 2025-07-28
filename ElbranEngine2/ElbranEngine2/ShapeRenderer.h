#pragma once
#include "IRenderer.h"
#include "Common.h"

class ShapeRenderer :
    public IRenderer
{
public:
    enum class Shape {
        Square,
        Circle,
        Triangle
    };

    Shape shape;
    Color color;

    ShapeRenderer();
    ShapeRenderer(Shape shape, Color color);

    void Draw() override;
};

