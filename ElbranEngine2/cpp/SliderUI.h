#pragma once
#include "UserInterface.h"
struct Transform;
struct Renderer;
struct RenderGroup;

class SliderUI : public UIElement {
public:
	Transform* root;
	Renderer* track;
	Renderer* mover;
	uint16_t segments;

	void (*onValueChanged)(SliderUI* slider, float newValue); // value is 0-1

	SliderUI() = default;
	void Initialize(RenderGroup* scene, float width, uint16_t segments, bool vertical = false);
	void SetValue(float newValue);
	float GetValue() const;

	void OnFocused() override;
	void OnUnfocused() override;
	void OnDisabled() override;
	void OnEnabled() override;
	void OnScrolled(float wheelDelta) override;
	void OnMouseDragged(Vector2 mousePosition, Vector2 mouseDelta) override;
	bool OnDirectionPressed(Direction direction) override;

private:
	bool vertical;
	float sliderWidth;
	float value; // 0-1
};

