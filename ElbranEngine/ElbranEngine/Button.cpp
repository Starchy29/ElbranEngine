#include "Button.h"
#include "Application.h"
#include "AssetManager.h"
#include "ColorRenderer.h"
#include "SpriteRenderer.h"
#include "Menu.h"

#define START_WIDTH 20
#define START_HEIGHT 6

Button::Button(ClickEffect clickFunc, Color baseColor, Color hoveredColor, Color disabledColor, std::string label)
	: GameObject(0, baseColor)
{
	type = ObjectTag::MenuButton;
	this->clickFunc = clickFunc;
	disabled = false;
	transform.SetScale(START_WIDTH, START_HEIGHT);

	textBox = new GameObject(1, label, APP->Assets()->arial, Color::Black);
	textBox->SetParent(this);
	textBox->GetTransform()->Scale(0.7f);

	useSprite = false;
	changeSprites = false;

	this->baseColor = baseColor;
	this->hoveredColor = hoveredColor;
	this->disabledColor = disabledColor;
	baseSprite = nullptr;
	hoveredSprite = nullptr;
	disabledSprite = nullptr;
}

Button::Button(ClickEffect clickFunc, bool translucent, std::shared_ptr<Sprite> baseSprite, Color hoveredColor, Color disabledColor, std::string label)
	: GameObject(0, baseSprite, translucent)
{
	type = ObjectTag::MenuButton;
	this->clickFunc = clickFunc;
	disabled = false;
	transform.SetScale(START_WIDTH, START_HEIGHT);

	if(label.size() > 0) {
		textBox = new GameObject(1, label, APP->Assets()->arial, Color::Black);
		textBox->SetParent(this);
		textBox->GetTransform()->Scale(0.7f);
	}

	useSprite = true;
	changeSprites = false;

	baseColor = Color::White;
	this->hoveredColor = hoveredColor;
	this->disabledColor = disabledColor;
	this->baseSprite = baseSprite;
	hoveredSprite = nullptr;
	disabledSprite = nullptr;
}

Button::Button(ClickEffect clickFunc, bool translucent, std::shared_ptr<Sprite> baseSprite, std::shared_ptr<Sprite> hoverSprite, std::shared_ptr<Sprite> disableSprite, std::string label)
	: GameObject(0, baseSprite, translucent)
{
	type = ObjectTag::MenuButton;
	this->clickFunc = clickFunc;
	disabled = false;
	transform.SetScale(START_WIDTH, START_HEIGHT);

	if(label.size() > 0) {
		textBox = new GameObject(1, label, APP->Assets()->arial, Color::Black);
		textBox->SetParent(this);
		textBox->GetTransform()->Scale(0.7f);
	}

	useSprite = true;
	changeSprites = true;

	baseColor = Color::White;
	hoveredColor = Color::White;
	disabledColor = Color::White;
	this->baseSprite = baseSprite;
	this->hoveredSprite = hoveredSprite;
	this->disabledSprite = disabledSprite;
}

void Button::SetHovered(bool hovered) {
	if(disabled) {
		return;
	}

	if(hovered) {
		// play sound
	}

	if(useSprite) {
		if(changeSprites) {
			((SpriteRenderer*)renderer)->sprite = hovered ? hoveredSprite : baseSprite;
		} else {
			((SpriteRenderer*)renderer)->tint = hovered ? hoveredColor : baseColor;
		}
	} else {
		((ColorRenderer*)renderer)->color = hovered ? hoveredColor : baseColor;
	}
}

void Button::SetDisabled(bool disabled) {
	this->disabled = disabled;
	if(useSprite) {
		if(changeSprites) {
			((SpriteRenderer*)renderer)->sprite = disabled ? disabledSprite : baseSprite;
		} else {
			((SpriteRenderer*)renderer)->tint = disabled ? disabledColor : baseColor;
		}
	} else {
		((ColorRenderer*)renderer)->color = disabled ? disabledColor : baseColor;
	}
}

void Button::Click() {
	// TODO: play sound
	clickFunc(this);
}

bool Button::IsDisabled() {
	return disabled;
}

GameObject* Button::GetLabel() {
	return textBox;
}

GameObject* Button::Clone() const {
	Button* copy = (Button*)Copy();
	if(scene != nullptr) {
		scene->Add(copy);
	}

	if(textBox != nullptr) {
		copy->textBox = textBox->Clone();
		copy->textBox->SetParent(copy);
	}

	for(GameObject* child : children) {
		if(child == textBox) {
			continue;
		}

		GameObject* childCopy = child->Clone();
		childCopy->SetParent(copy);
	}

	return copy;
}

GameObject* Button::Copy() const {
	return new Button(*this);
}

Button::Button(const Button& original)
	: GameObject(original)
{
	baseColor = original.baseColor;
	hoveredColor = original.hoveredColor;
	disabledColor = original.disabledColor;
	baseSprite = original.baseSprite;
	hoveredSprite = original.hoveredSprite;
	disabledSprite = original.disabledSprite;
	clickFunc = original.clickFunc;

	useSprite = original.useSprite;
	changeSprites = original.changeSprites;
	disabled = original.disabled;
	// textbox set in Clone()
}
