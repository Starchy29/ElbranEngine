#include "Button.h"
#include "RenderGroup.h"
#include "Game.h"

void Button::Initialize(RenderGroup* scene) {
	normalColor = Color::White;
	hoveredColor = Color(0.5f, 0.5f, 0.5f);

	box = scene->ReserveRenderer();
	box->InitShape(PrimitiveShape::Square, normalColor);
}

AlignedRect Button::GetScreenArea() const {
	// assumes button has no world rotation
	return AlignedRect(box->transform->position, box->transform->scale);
}

float Button::GetScreenOrder() const {
	return box->transform->zOrder;
}

bool Button::ProcessInput(InputEventUI inputEvent, AppComponents app) {
	switch(inputEvent.type) {
	case InputEventUI::Type::ControllerFocused:
	case InputEventUI::Type::MouseHovered:
		if(disabled) return false;
		box->shapeData.color = hoveredColor;
		return true;
	case InputEventUI::Type::Unfocused:
		if(disabled) return false;
		box->shapeData.color = normalColor;
		return true;
	case InputEventUI::Type::ControllerSelected:
	case InputEventUI::Type::MouseClicked:
		if(disabled) return false;
		clickFunction(this, app);
		return true;
	}
	return false;
}

void Button::Disable() {
	disabled = true;
	box->shapeData.color = normalColor;
	box->shapeData.color.alpha = 0.5f;
}

void Button::Enable() {
	disabled = false;
	box->shapeData.color.alpha = 1.0f;
}