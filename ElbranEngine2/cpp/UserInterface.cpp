#include "UserInterface.h"
#include "InputManager.h"
#include "RenderGroup.h"

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
	if(gamepadEnabled && inputs->controllers) {
		bool inputUsed = false;
		if(inputs->controllers[0].JustPressed((uint32_t)InputAction::Up)) {
			if(focus) {
				focus->OnDirectionPressed(Direction::Up, &inputUsed);
				if(!inputUsed) ChangeFocus(FindClosest(Vector2::Up, sceneCamera));
			} else ChangeFocus(FindFurthest(Vector2::Up));
		}
		else if(inputs->controllers[0].JustPressed((uint32_t)InputAction::Down)) {
			if(focus) {
				focus->OnDirectionPressed(Direction::Down, &inputUsed);
				if(!inputUsed) ChangeFocus(FindClosest(Vector2::Down, sceneCamera));
			} else ChangeFocus(FindFurthest(Vector2::Down));
		}
		else if(inputs->controllers[0].JustPressed((uint32_t)InputAction::Left)) {
			if(focus) {
				focus->OnDirectionPressed(Direction::Left, &inputUsed);
				if(!inputUsed) ChangeFocus(FindClosest(Vector2::Left, sceneCamera));
			} else ChangeFocus(FindFurthest(Vector2::Left));
		}
		else if(inputs->controllers[0].JustPressed((uint32_t)InputAction::Right)) {
			if(focus) {
				focus->OnDirectionPressed(Direction::Right, &inputUsed);
				if(!inputUsed) ChangeFocus(FindClosest(Vector2::Right, sceneCamera));
			} else ChangeFocus(FindFurthest(Vector2::Right));
		}
	}

	if(mouseEnabled) {
		Vector2 mouseDelta = inputs->GetMouseWorldDisplacement(sceneCamera);
		Vector2 mousePos = inputs->GetMouseWorldPosition(sceneCamera);

		// click and drag
		if(focus && (mouseDelta != Vector2::Zero && inputs->mouse.leftClicked || inputs->WasMouseClicked(true))) {
			focus->OnMouseDragged(mousePos, mouseDelta);
		}

		// check for a new hovered element
		if(mouseDelta != Vector2::Zero) {
			UIElement* hovered = nullptr;
			AlignedRect unitSquare = AlignedRect(Vector2::Zero, Vector2(1.f, 1.f));
			for(uint16_t i = 0; i < numElements; i++) {
				Vector2 normalizedMouse = elements[i]->selectArea->Inverse() * mousePos;
				if(unitSquare.Contains(normalizedMouse)) {
					hovered = elements[i];
					break;
				}
			}

			ChangeFocus(hovered);
		}

		// scroll the focused element with the mouse wheel
		float scroll = inputs->mouse.wheelDelta;
		if(scroll != 0.f && focus) {
			focus->OnScrolled(scroll);
		}
	}

	// select the focused element
	if(focus && inputs->controllers && inputs->controllers[0].JustPressed((uint32_t)InputAction::Select)) {
		focus->OnSelected();
	}
}

void UserInterface::Join(UIElement* element) {
	elements[numElements] = element;
	numElements++;
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
	if(focus) focus->OnUnfocused();
	focus = newFocus;
	if(focus) focus->OnFocused();
}