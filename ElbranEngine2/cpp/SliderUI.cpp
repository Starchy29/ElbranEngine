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
	Transform* selectTransform = scene->ReserveTransform(&selectArea);
	selectTransform->parent = root;
	track->transform->parent = selectTransform;
	mover->transform->parent = root;
	mover->transform->zOrder = -0.1f;

	if(vertical) {
		selectTransform->scale.y = width;
		track->transform->scale.x = 0.3f;

	} else {
		selectTransform->scale.x = width;
		track->transform->scale.y = 0.3f;
	}
}

void SliderUI::SetValue(float newValue) {
	newValue = Math::Clamp(newValue, 0.f, 1.f);
	newValue = Math::Round(newValue * segments) / segments;
	if(vertical) {
		mover->transform->position.y = Tween::Lerp(-sliderWidth, sliderWidth, newValue) / 2.0f;
	} else {
		mover->transform->position.x = Tween::Lerp(-sliderWidth, sliderWidth, newValue) / 2.0f;
	}

	if(newValue != value) {
		value = newValue;
		if(onValueChanged) onValueChanged(this, value);
	}
}

float SliderUI::GetValue() const {
	return value;
}

void SliderUI::OnFocused() {
	mover->shapeData.color = Color(0.5f, 0.5f, 0.5f);
}

void SliderUI::OnUnfocused() {
	mover->shapeData.color = Color::White;
}

void SliderUI::OnDisabled() {
	track->shapeData.color.alpha = 0.5f;
	mover->shapeData.color.alpha = 0.5f;
}

void SliderUI::OnEnabled() {
	track->shapeData.color.alpha = 1.f;
	mover->shapeData.color.alpha = 1.f;
}

void SliderUI::OnScrolled(float wheelDelta) {
	SetValue(value + wheelDelta / segments);
}

void SliderUI::OnMouseDragged(Vector2 mousePosition, Vector2 mouseDelta) {
	Vector2 normalizedPos = selectArea->Inverse() * mousePosition;
	if(vertical) {
		SetValue(normalizedPos.y + 0.5f);
	} else {
		SetValue(normalizedPos.x + 0.5f);
	}
}

bool SliderUI::OnDirectionPressed(Direction direction) {
	if(vertical) {
		if(direction == Direction::Right || direction == Direction::Left) return false;
		SetValue(value + 1.0f / segments * (direction == Direction::Up ? 1.f : -1.f));
		return true;
	} 

	// horizontal
	if(direction == Direction::Up || direction == Direction::Down) return false;
	SetValue(value + 1.0f / segments * (direction == Direction::Right ? 1.f : -1.f));
	return true;
}