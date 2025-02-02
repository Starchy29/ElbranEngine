#pragma once
#include "IRenderer.h"
#include "Color.h"
#include "Mesh.h"

class ColorRenderer :
    public IRenderer
{
public:
    enum Shape {
        Square,
        Circle,
        Triangle
    };

    Color color;

    ColorRenderer(Color color, Shape shape);

    void Draw(Camera* camera, const Transform& transform) override;
    IBehavior* Clone() override;

private:
    std::shared_ptr<Mesh> mesh;
};

