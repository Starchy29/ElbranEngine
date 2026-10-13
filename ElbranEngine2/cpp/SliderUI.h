#pragma once
#include "UserInterface.h"
struct Transform;
struct Renderer;
struct RenderGroup;

class SliderUI : public UIElement {
public:
	void(*sliderEffect)(float value, SliderUI* slid, AppComponents);
	Transform* root;
	Renderer* track;
	Renderer* mover;
	uint16_t segments;

	SliderUI() = default;
	void Initialize(RenderGroup* scene, float width, uint16_t segments, bool vertical = false);
	void SetValue(float newValue, AppComponents);
	float GetValue() const;

	AlignedRect GetScreenArea() const override;
	float GetScreenOrder() const override;
	bool ProcessInput(InputEventUI inputEvent, AppComponents) override;

private:
	bool vertical;
	float sliderWidth;
	float value; // 0-1
};

