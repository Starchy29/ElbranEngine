#pragma once
#include "IRenderer.h"
#include <SpriteFont.h>
#include "Color.h"
#include "Enums.h"

class TextRenderer :
    public IRenderer
{
public:
    std::string text;
    std::shared_ptr<DirectX::DX11::SpriteFont> font;
    Color color;
    Direction horizontalAlignment;
    Direction verticalAlignment;

    TextRenderer(std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color);

    void Draw(Camera* camera, const Transform& transform) override;
    IBehavior* Clone() override;
};

