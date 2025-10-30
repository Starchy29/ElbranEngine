#include "SliderUI.h"
#include "InputManager.h"
#include "Application.h"

void SliderUI::Initialize(Scene* scene, float width, uint16_t segments, bool vertical) {
	this->vertical = vertical;
	this->segments = segments;
	this->scene = scene;
	sliderWidth = width;
	value = 0.5f;

	track.Initialize(ShapeRenderer::Shape::Square, Color::White);
	mover.Initialize(ShapeRenderer::Shape::Square, Color::White);

	scene->ReserveTransform(&root, nullptr);
	Transform* selectTransform;
	scene->ReserveTransform(&selectTransform, &selectArea);
	scene->AddRenderer(&track, true);
	scene->AddRenderer(&mover, true);
	selectTransform->parent = root;
	track.transform->parent = selectTransform;
	mover.transform->parent = root;
	mover.transform->zOrder = -0.1f;

	if(vertical) {
		selectTransform->scale.y = width;
		track.transform->scale.x = 0.3f;

	} else {
		selectTransform->scale.x = width;
		track.transform->scale.y = 0.3f;
	}
}

void SliderUI::SetValue(float newValue) {
	newValue = Math::Clamp(newValue, 0.f, 1.f);
	newValue = roundf(newValue * segments) / segments;
	if(vertical) {
		mover.transform->position.y = Tween::Lerp(-sliderWidth, sliderWidth, newValue) / 2.0f;
	} else {
		mover.transform->position.x = Tween::Lerp(-sliderWidth, sliderWidth, newValue) / 2.0f;
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
	mover.color = Color(0.5f, 0.5f, 0.5f);
}

void SliderUI::OnUnfocused() {
	mover.color = Color::White;
}

void SliderUI::OnDisabled() {
	track.color.alpha = 0.5f;
	mover.color.alpha = 0.5f;
}

void SliderUI::OnEnabled() {
	track.color.alpha = 1.f;
	mover.color.alpha = 1.f;
}

void SliderUI::OnScrolled(float wheelDelta) {
	SetValue(value + wheelDelta / segments);
}

void SliderUI::OnMouseDragged(Vector2 mouseDelta) {
	Vector2 normalizedPos = selectArea->Inverse() * app->input->GetMousePosition(&scene->camera);
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
