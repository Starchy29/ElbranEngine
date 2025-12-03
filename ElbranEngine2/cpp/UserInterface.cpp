#include "UserInterface.h"
#include "InputManager.h"
#include "Renderer.h"

bool UserInterface::mouseEnabled = true;
bool UserInterface::gamepadEnabled = true;

void UserInterface::Initialize(uint16_t maxElements) {
	mouseEnabled = true;
	gamepadEnabled = true;
	focus = nullptr;

	elements = new UIElement*[maxElements];
}

void UserInterface::Release() {
	delete[] elements;
}

void UserInterface::Update(const InputManager* inputs, float deltaTime, const Camera* sceneCamera) {
	if(gamepadEnabled) {
		if(inputs->JustPressed(InputAction::Up)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Up);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Up, sceneCamera));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Up));
			}
		}
		else if(inputs->JustPressed(InputAction::Down)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Down);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Down, sceneCamera));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Down));
			}
		}
		else if(inputs->JustPressed(InputAction::Left)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Left);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Left, sceneCamera));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Left));
			}
		}
		else if(inputs->JustPressed(InputAction::Right)) {
			if(focus) {
				bool inputUsed = focus->OnDirectionPressed(Direction::Right);
				if(!inputUsed) {
					ChangeFocus(FindClosest(Vector2::Right, sceneCamera));
				}
			} else {
				ChangeFocus(FindFurthest(Vector2::Right));
			}
		}
	}

	if(mouseEnabled) {
		Vector2 mouseDelta = inputs->GetMouseDelta(sceneCamera);

		// click and drag
		if(focus && (mouseDelta != Vector2::Zero && inputs->IsPressed(InputAction::LeftCLick) || inputs->JustPressed(InputAction::LeftCLick))) {
			focus->OnMouseDragged(inputs->GetMousePosition(sceneCamera), mouseDelta);
		}

		// check for a new hovered element
		if(mouseDelta != Vector2::Zero) {
			Vector2 mousePos = inputs->GetMousePosition(sceneCamera);
			UIElement* hovered = nullptr;
			AlignedRect unitSquare = AlignedRect(Vector2::Zero, Vector2(1.f, 1.f));
			for(uint16_t i = 0; i < numElements - numDisabled; i++) {
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
	elements[numElements] = elements[numElements - numDisabled];
	elements[numElements - numDisabled] = element;
	numElements++;
}

void UserInterface::SetEnabled(UIElement* element, bool enabled) {
	int16_t index = -1;
	for(int16_t i = 0; i < numElements; i++) {
		if(elements[i] == element) {
			index = i;
			break;
		}
	}
	if(index < 0) return;

	if(enabled) {
		if(index < numElements - numDisabled) return;
		elements[index] = elements[numElements - numDisabled];
		elements[numElements - numDisabled] = element;
		numDisabled--;
		element->OnEnabled();
	} else {
		if(index >= numElements - numDisabled) return;
		elements[index] = elements[numElements - numDisabled - 1];
		elements[numElements - numDisabled - 1] = element;
		numDisabled++;
		if(focus == element) ChangeFocus(nullptr);
		element->OnDisabled();
	}
}

UIElement* UserInterface::FindFurthest(Vector2 direction) {
	UIElement* furthest = nullptr;
	float maxDistance;
	for(uint16_t i = 0; i < numElements; i++) {
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

UIElement* UserInterface::FindClosest(Vector2 direction, const Camera* sceneCamera) {
	// assumes focus is non-null
	Vector2 screenSize = sceneCamera->GetWorldDimensions();
	Vector2 start = *focus->selectArea * Vector2::Zero;
	float minDistance;
	UIElement* closest = nullptr;
	for(uint16_t i = 0; i < numElements; i++) {
		Vector2 position = *elements[i]->selectArea * Vector2::Zero;
		float dotProd = (position - start).Dot(direction);
		if(Math::Abs(dotProd) < 0.1f) {
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
