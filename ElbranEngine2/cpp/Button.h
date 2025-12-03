#pragma once
#include "UserInterface.h"
#include "Renderer.h"

struct RenderGroup;

class Button : public UIElement {
public:
	void (*clickEffect)(Button* clicked);
	Renderer* box;

	Color normalColor;
	Color hoveredColor;

	Button() = default;
	void Initialize(RenderGroup* scene, void (*clickEffect)(Button*));

	void OnFocused() override;
	void OnUnfocused() override;
	void OnDisabled() override;
	void OnEnabled() override;
	void OnSelected() override;
};

