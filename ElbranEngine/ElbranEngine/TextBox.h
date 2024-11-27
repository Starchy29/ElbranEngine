#pragma once
#include "GameObject.h"
#include <SpriteFont.h>

class TextBox :
    public GameObject
{
public:
    std::string text;
    std::shared_ptr<DirectX::DX11::SpriteFont> font;
    float maxSize;
    Direction horizontalAlignment;
    Direction verticalAlignment;

    TextBox(Scene* scene, float zCoord, std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color);

    virtual void Draw(Camera* camera) override;

private:
    
};

