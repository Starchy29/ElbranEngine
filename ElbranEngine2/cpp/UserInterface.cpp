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

void UserInterface::Update(AppComponents app, float deltaTime, const Camera* sceneCamera) {
	InputEventUI inputEvent;
	if(gamepadEnabled && app.input->controllers) {
		if(focus) {
			// check if player pressed the select button
			if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Select)) {
				focus->ProcessInput({ InputEventUI::Type::ControllerSelected }, app);
				return;
			}

			// check if player pressed a direction
			Direction inputDirection = Direction::None;
			if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Up)) inputDirection = Direction::Up;
			else if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Down)) inputDirection = Direction::Down;
			else if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Left)) inputDirection = Direction::Left;
			else if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Right)) inputDirection = Direction::Right;

			inputEvent.type = InputEventUI::Type::DirectionPressed;
			inputEvent.direction = inputDirection;
			if(inputDirection != Direction::None) {
				bool focusUsedInput = focus->ProcessInput(inputEvent, app);
				if(!focusUsedInput) {
					// focus on the next adjacent element
					Vector2 directionVector;
					switch(inputDirection) {
					case Direction::Up: directionVector = Vector2::Up; break;
					case Direction::Down: directionVector = Vector2::Down; break;
					case Direction::Left: directionVector = Vector2::Left; break;
					case Direction::Right: directionVector = Vector2::Right; break;
					}

					UIElement* newFocus = focus;
					for(uint16_t i = 0; i < numElements; i++) {
						newFocus = FindAdjacentElement(newFocus, directionVector, sceneCamera);
						if(newFocus == nullptr || newFocus == focus) break;
						bool focusAllowed = newFocus->ProcessInput({ InputEventUI::Type::ControllerFocused }, app);
						if(focusAllowed) {
							focus->ProcessInput({ InputEventUI::Type::Unfocused }, app);
							focus = newFocus;
							break;
						}
					}
				}
			}
		}
		else if(numElements > 0) {
			// when there is no controller focus, focus on the furthest element in the direction the user presses
			Vector2 pressedVector = Vector2::Zero;
			if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Up)) pressedVector = Vector2::Up;
			else if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Down)) pressedVector = Vector2::Down;
			else if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Left)) pressedVector = Vector2::Left;
			else if(app.input->controllers[0].JustPressed((uint32_t)InputAction::Right)) pressedVector = Vector2::Right;

			if(pressedVector != Vector2::Zero) {
				UIElement* furthest = nullptr;
				float maxDistance;
				for(uint16_t i = 0; i < numElements; i++) {
					Vector2 position = elements[i]->GetScreenArea().Center();
					Vector2 weighted = position * pressedVector;
					float distance = weighted.x + weighted.y;
					if(furthest == nullptr || distance > maxDistance) {
						furthest = elements[i];
						maxDistance = distance;
					}
				}

				pressedVector *= -1.f;
				for(uint16_t i = 0; i < numElements; i++) {
					bool focusSucceeded = furthest->ProcessInput({ InputEventUI::Type::ControllerFocused }, app);
					if(focusSucceeded) {
						focus = furthest;
						break;
					}
					furthest = FindAdjacentElement(furthest, pressedVector, sceneCamera);
				}
			}
		}
	}

	if(mouseEnabled) {
		Vector2 mouseDelta = app.input->GetMouseWorldDisplacement(sceneCamera);
		Vector2 mousePos = app.input->GetMouseWorldPosition(sceneCamera);
		UIElement** sortedElements = (UIElement**)app.arena->Reserve(sizeof(UIElement*) * numElements);
		uint32_t numSorted = 0;

		// click and drag
		if(focus && mouseDelta != Vector2::Zero && app.input->mouse.leftClicked && app.input->mouse.lastFrame.leftClicked) {
			inputEvent.type = InputEventUI::Type::Dragged;
			inputEvent.mousePos = mousePos;
			inputEvent.mouseDelta = mouseDelta;
			bool dragUsed = focus->ProcessInput(inputEvent, app);
			if(dragUsed) return;
		}

		// determine which elements are currently hovered and sort them by z order
		for(uint16_t i = 0; i < numElements; i++) {
			float newScreenOrder = elements[i]->GetScreenOrder();
			if(!elements[i]->GetScreenArea().Contains(mousePos)) continue;
			uint16_t insertIndex;
			for(insertIndex = numSorted; insertIndex > 0; insertIndex--) {
				if(newScreenOrder >= sortedElements[insertIndex - 1]->GetScreenOrder()) break;
				sortedElements[insertIndex] = sortedElements[insertIndex - 1];
			}
			sortedElements[insertIndex] = elements[i];
			numSorted++;
		}

		// check for a new hovered element
		if(mouseDelta != Vector2::Zero) {
			UIElement* hovered = nullptr;
			inputEvent.type = InputEventUI::Type::MouseHovered;
			inputEvent.mousePos = mousePos;
			for(uint32_t i = 0; i < numSorted; i++) {
				bool hoverAccepted = false;
				if(sortedElements[i] == focus) hoverAccepted = true;
				else hoverAccepted = sortedElements[i]->ProcessInput(inputEvent, app);
				if(hoverAccepted) {
					hovered = sortedElements[i];
					break;
				}
			}

			if(hovered != focus) {
				if(focus) focus->ProcessInput({ InputEventUI::Type::Unfocused }, app);
				focus = hovered;
			}
		}

		// try clicking or scrolling on a hovered element
		if(app.input->mouse.wheelDelta != 0.f) {
			inputEvent.type = InputEventUI::Type::Scrolled;
			inputEvent.scrollDelta = app.input->mouse.wheelDelta;
		}
		else if(app.input->WasMouseClicked(true)) {
			inputEvent.type = InputEventUI::Type::MouseClicked;
			inputEvent.mousePos = mousePos;
		}
		if(numSorted > 0) {
			for(uint16_t i = 0; i < numSorted; i++) {
				// attempt the input on each hovered element from front to back
				bool usedInput = sortedElements[i]->ProcessInput(inputEvent, app);
				if(usedInput) break;
			}
		}
	}
}

void UserInterface::Join(UIElement* element) {
	elements[numElements] = element;
	numElements++;
}

UIElement* UserInterface::FindAdjacentElement(UIElement* closestFrom, Vector2 direction, const Camera* sceneCamera) {
	// assumes focus is non-null
	Vector2 screenSize = sceneCamera->GetWorldDimensions();
	Vector2 start = closestFrom->GetScreenArea().Center();
	float minDistance;
	UIElement* closest = nullptr;
	for(uint16_t i = 0; i < numElements; i++) {
		Vector2 position = elements[i]->GetScreenArea().Center();
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