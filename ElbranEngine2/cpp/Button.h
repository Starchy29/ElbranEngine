#pragma once
#include "UserInterface.h"

struct Renderer;
struct RenderGroup;

class Button : public UIElement {
public:
	void(*clickFunction)(Button* clicked, AppComponents);
	Renderer* box;
	Color normalColor;
	Color hoveredColor;
	bool disabled;

	Button() = default;
	void Initialize(RenderGroup* scene);

	AlignedRect GetScreenArea() const override;
	float GetScreenOrder() const override;
	bool ProcessInput(InputEventUI inputEvent, AppComponents) override;

	void Disable();
	void Enable();
};

