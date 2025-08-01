#pragma once
#include "UserInterface.h"
#include "ShapeRenderer.h"
#include "Scene.h"

class SliderUI 
	: public UIElement
{
public:
	Scene* scene;
	Transform* root;
	ShapeRenderer track;
	ShapeRenderer mover;
	unsigned int segments;

	void (*onValueChanged)(SliderUI* slider, float newValue); // value is 0-1

	SliderUI(Scene* scene, float width, unsigned int segments, bool vertical = false);
	void SetValue(float newValue);
	float GetValue() const;

	void OnFocused() override;
	void OnUnfocused() override;
	void OnDisabled() override;
	void OnEnabled() override;
	void OnScrolled(float wheelDelta) override;
	void OnMouseDragged(Vector2 mouseDelta) override;
	bool OnDirectionPressed(Direction direction) override;

private:
	bool vertical;
	float sliderWidth;
	float value; // 0-1
};

