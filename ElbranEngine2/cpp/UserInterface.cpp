#include "UserInterface.h"
#include "Application.h"
#include "InputManager.h"

void UserInterface::Initialize(Scene* scene, uint16_t maxElements) {
	this->scene = scene;
	mouseEnabled = true;
	gamepadEnabled = true;
	focus = nullptr;

	elements.Allocate(maxElements);
	disabled.Allocate(maxElements);
}

void UserInterface::Release() {
	elements.Release();
	disabled.Release();
}

void UserInterface::Update(float deltaTime) {
	InputManager* inputs = app->input;

	if(gamepadEnabled) {
		if(inputs->JustPressed(InputAction::Up)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Up);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Up));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Up));
			}
		}
		else if(inputs->JustPressed(InputAction::Down)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Down);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Down));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Down));
			}
		}
		else if(inputs->JustPressed(InputAction::Left)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Left);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Left));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Left));
			}
		}
		else if(inputs->JustPressed(InputAction::Right)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Right);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Right));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Right));
			}
		}
	}

	if(mouseEnabled) {
		Vector2 mouseDelta = inputs->GetMouseDelta(&scene->camera);

		// click and drag
		if(focus && (mouseDelta != Vector2::Zero && inputs->IsPressed(InputAction::LeftCLick) || inputs->JustPressed(InputAction::LeftCLick))) {
			focus->OnMouseDragged(mouseDelta);
		}

		// check for a new hovered element
		if(mouseDelta != Vector2::Zero) {
			Vector2 mousePos = inputs->GetMousePosition(&scene->camera);
			UIElement* hovered = nullptr;
			AlignedRect unitSquare = AlignedRect(Vector2::Zero, Vector2(1.f, 1.f));
			for(int i = 0; i < elements.Size(); i++) {
				Vector2 normalizedMouse = elements[i]->selectArea->Inverse() * mousePos;
				if(unitSquare.Contains(normalizedMouse)) {
					hovered = elements[i];
					break;
				}
			}

			ChangeFocus(hovered);
		}

		// scroll the focused element with the mouse wheel
		float scroll = inputs->GetMouseWheelSpin();
		if(scroll != 0.f && focus) {
			focus->OnScrolled(scroll);
		}
	}

	// select the focused element
	if(focus && inputs->JustPressed(InputAction::Select)) {
		focus->OnSelected();
	}
}

void UserInterface::Join(UIElement* element) {
	elements.Add(element);
}

void UserInterface::SetEnabled(UIElement* element, bool enabled) {
	if(enabled) {
		int16_t index = disabled.IndexOf(element);
		if(index < 0) return;
		disabled.RemoveAt(index);
		elements.Add(element);
		element->OnEnabled();
	} else {
		int16_t index = elements.IndexOf(element);
		if(index < 0) return;
		elements.RemoveAt(index);
		disabled.Add(element);
		if(focus == element) {
			ChangeFocus(nullptr);
		}
		element->OnDisabled();
	}
}

UIElement* UserInterface::FindFurthest(Vector2 direction) {
	UIElement* furthest = nullptr;
	float maxDistance;
	uint32_t numElements = elements.Size();
	for(uint32_t i = 0; i < numElements; i++) {
		Vector2 position = *elements[i]->selectArea * Vector2::Zero;
		Vector2 weighted = position * direction;
		float distance = weighted.x + weighted.y;
		if(furthest == nullptr || distance > maxDistance) {
			furthest = elements[i];
			maxDistance = distance;
		}
	}

	return furthest;
}

UIElement* UserInterface::FindClosest(Vector2 direction) {
	// assumes focus is non-null
	Vector2 screenSize = scene->camera.GetWorldDimensions();
	Vector2 start = *focus->selectArea * Vector2::Zero;
	float minDistance;
	UIElement* closest = nullptr;
	uint32_t numElements = elements.Size();
	for(uint16_t i = 0; i < numElements; i++) {
		Vector2 position = *elements[i]->selectArea * Vector2::Zero;
		float dotProd = (position - start).Dot(direction);
		if(abs(dotProd) < 0.1f) {
			continue;
		}

		if(dotProd < 0.f) {
			// wrap around
			position += direction * screenSize;
		}

		float distance = position.SquareDistance(start);
		if(closest == nullptr || distance < minDistance) {
			closest = elements[i];
			minDistance = distance;
		}
	}

	return closest;
}

void UserInterface::ChangeFocus(UIElement* newFocus) {
	if(newFocus == focus) return;

	if(focus) {
		focus->OnUnfocused();
	}
	focus = newFocus;
	if(focus) {
		focus->OnFocused();
	}
}
