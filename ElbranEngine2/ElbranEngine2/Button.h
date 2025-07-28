#pragma once
#include "UserInterface.h"
#include "ShapeRenderer.h"
#include "Scene.h"

class Button 
	: public UIElement
{
public:
	void (*clickEffect)(Button* clicked);
	ShapeRenderer box;

	Color normalColor;
	Color hoveredColor;

	Button(Scene* scene, void (*clickEffect)(Button*));

	void OnFocused() override;
	void OnUnfocused() override;
	void OnSelected() override;
};

