#pragma once
#include "GameObject.h"
#include "Color.h"
#include "Sprite.h"

class Button;
typedef void (*ClickEffect)(Button* clicked);

class Button :
    public GameObject
{
public:
    Color baseColor;
    Color hoveredColor;
    Color disabledColor;
    std::shared_ptr<Sprite> baseSprite;
    std::shared_ptr<Sprite> hoveredSprite;
    std::shared_ptr<Sprite> disabledSprite;
    ClickEffect clickFunc;

    Button(ClickEffect clickFunc, Color baseColor, Color hoveredColor, Color disabledColor, std::string label);
    Button(ClickEffect clickFunc, bool translucent, std::shared_ptr<Sprite> baseSprite, Color hoveredColor, Color disabledColor, std::string label = "");
    Button(ClickEffect clickFunc, bool translucent, std::shared_ptr<Sprite> baseSprite, std::shared_ptr<Sprite> hoverSprite, std::shared_ptr<Sprite> disableSprite, std::string label = "");

    virtual void SetHovered(bool hovered);
    virtual void SetDisabled(bool disabled);
    virtual void Click();
    bool IsDisabled();
    GameObject* GetLabel();
    virtual GameObject* Clone() const override;

protected:
    virtual GameObject* Copy() const override;
    Button(const Button& original);

private:
    bool useSprite;
    bool changeSprites;
    bool disabled;
    GameObject* textBox;
};

