#include "Button.h"
#include "Scene.h"

void Button::Initialize(Scene* scene, void (*clickEffect)(Button*)) {
	this->clickEffect = clickEffect;
	normalColor = Color::White;
	hoveredColor = Color(0.5f, 0.5f, 0.5f);

	scene->AddRenderer(&box, true);
	selectArea = box.worldMatrix;
}

void Button::OnFocused() {
	box.color = hoveredColor;
}

void Button::OnUnfocused() {
	box.color = normalColor;
}

void Button::OnDisabled() {
	box.color.alpha = 0.5f;
}

void Button::OnEnabled() {
	box.color.alpha = 1.0f;
}

void Button::OnSelected() {
	clickEffect(this);
}
