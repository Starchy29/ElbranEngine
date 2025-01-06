#pragma once
#include "IRenderer.h"
#include "Color.h"

// doesn't actually render anything, acts as a container for light data that attaches to a game object
class LightRenderer :
    public IRenderer
{
public:
    Color color;
    float radius;
    float brightness;
    float coneSize;

    LightRenderer(Color color, float radius, float brightness);

    virtual void Draw(Camera* camera, const Transform& transform) override;
    virtual IBehavior* Clone() override;
};

