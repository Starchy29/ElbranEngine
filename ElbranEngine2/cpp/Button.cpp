#include "Button.h"
#include "RenderGroup.h"

void Button::Initialize(RenderGroup* scene, void (*clickEffect)(Button*)) {
	this->clickEffect = clickEffect;
	normalColor = Color::White;
	hoveredColor = Color(0.5f, 0.5f, 0.5f);

	box = scene->ReserveRenderer();
	box->InitShape(PrimitiveShape::Square, normalColor);
	selectArea = box->worldMatrix;
}

void Button::OnFocused() {
	box->shapeData.color = hoveredColor;
}

void Button::OnUnfocused() {
	box->shapeData.color = normalColor;
}

void Button::OnDisabled() {
	box->shapeData.color.alpha = 0.5f;
}

void Button::OnEnabled() {
	box->shapeData.color.alpha = 1.0f;
}

void Button::OnSelected() {
	clickEffect(this);
}