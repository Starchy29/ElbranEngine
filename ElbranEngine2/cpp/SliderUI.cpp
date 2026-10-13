#include "SliderUI.h"
#include "InputManager.h"
#include "Application.h"
#include "RenderGroup.h"

void SliderUI::Initialize(RenderGroup* scene, float width, uint16_t segments, bool vertical) {
	this->vertical = vertical;
	this->segments = segments;
	sliderWidth = width;
	value = 0.5f;

	track = scene->ReserveRenderer();
	mover = scene->ReserveRenderer();
	track->InitShape(PrimitiveShape::Square, Color::White);
	mover->InitShape(PrimitiveShape::Square, Color::White);

	root = scene->ReserveTransform();
	track->transform->parent = root;
	mover->transform->parent = root;
	mover->transform->zOrder = -0.1f;

	if(vertical) {
		track->transform->scale.y = width;
		track->transform->scale.x = 0.3f;
	} else {
		track->transform->scale.x = width;
		track->transform->scale.y = 0.3f;
	}
}

void SliderUI::SetValue(float newValue, AppComponents app) {
	newValue = Math::Clamp(newValue, 0.f, 1.f);
	newValue = Math::Round(newValue * segments) / segments;
	if(newValue == value) return;
	float sliderPos = Tween::Lerp(-sliderWidth, sliderWidth, newValue) / 2.0f;
	if(vertical) mover->transform->position.y = sliderPos;
	else mover->transform->position.x = sliderPos;
	value = newValue;
	sliderEffect(value, this, app);
}

float SliderUI::GetValue() const {
	return value;
}

AlignedRect SliderUI::GetScreenArea() const {
	Vector2 center = root->position;
	Vector2 size;
	if(vertical) size = Vector2(mover->transform->scale.x, track->transform->scale.y);
	else size = Vector2(track->transform->scale.x, mover->transform->scale.y);
	return AlignedRect(center, size);
}

float SliderUI::GetScreenOrder() const {
	return root->zOrder;
}

bool SliderUI::ProcessInput(InputEventUI inputEvent, AppComponents app) {
	AlignedRect screenBox;
	switch(inputEvent.type) {
	case InputEventUI::Type::ControllerFocused:
	case InputEventUI::Type::MouseHovered:
		mover->shapeData.color = Color(0.5f, 0.5f, 0.5f);
		return true;
	case InputEventUI::Type::Unfocused:
		mover->shapeData.color = Color::White;
		return true;
	case InputEventUI::Type::Scrolled:
		SetValue(value + inputEvent.scrollDelta / segments, app);
		return true;
	case InputEventUI::Type::MouseClicked:
	case InputEventUI::Type::Dragged:
		screenBox = GetScreenArea();
		float position, maxPosition, minPosition;
		if(vertical) {
			position = inputEvent.mousePos.y;
			maxPosition = screenBox.top;
			minPosition = screenBox.bottom;
		} else {
			position = inputEvent.mousePos.x;
			maxPosition = screenBox.right;
			minPosition = screenBox.left;
		}
		SetValue((position - minPosition) / (maxPosition - minPosition), app);
		return true;
	case InputEventUI::Type::DirectionPressed:
		if(vertical) {
			if(inputEvent.direction == Direction::Right || inputEvent.direction == Direction::Left) return false;
			SetValue(value + 1.0f / segments * (inputEvent.direction == Direction::Up ? 1.f : -1.f), app);
		} else { // horizontal
			if(inputEvent.direction == Direction::Up || inputEvent.direction == Direction::Down) return false;
			SetValue(value + 1.0f / segments * (inputEvent.direction == Direction::Right ? 1.f : -1.f), app);
		}
		return true;
	}
	return false;
}