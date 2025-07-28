#include "Button.h"
#include "Scene.h"

Button::Button(Scene* scene, void (*clickEffect)(Button*)) {
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

void Button::OnSelected() {
	clickEffect(this);
}
