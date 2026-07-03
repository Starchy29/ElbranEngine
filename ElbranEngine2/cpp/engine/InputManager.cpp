#include "..\InputManager.h"
#include "..\RenderGroup.h"

#ifdef WINDOWS
#include "WindowsInput.h"
#endif

void InputManager::Initialize(PlatformInput* platformInput) {
	this->platformInput = platformInput;
}

void InputManager::Release() {
	delete platformInput;
}

void InputManager::Update(float deltaTime, UInt2 viewDimensions, UInt2 viewOffset) {
	platformInput->CheckInputs();
	typedKey = platformInput->GetTypedKey();

	// update mouse
	mouse.lastFrame.screenPos = mouse.screenPos;
	mouse.lastFrame.leftClicked = mouse.leftClicked;
	mouse.lastFrame.rightClicked = mouse.rightClicked;
	mouse.lastFrame.middleClicked = mouse.middleClicked;
	mouse.screenPos = platformInput->GetMouseScreenPosition(viewDimensions, viewOffset);
	mouse.wheelDelta = platformInput->DetermineMouseSpin();
	mouse.leftClicked = platformInput->IsKeyPressed(KEY_MOUSE_LEFT, 0);
	mouse.rightClicked = platformInput->IsKeyPressed(KEY_MOUSE_RIGHT, 0);
	mouse.middleClicked = platformInput->IsKeyPressed(KEY_MOUSE_MIDDLE, 0);

	// update player controllers
	for(uint32_t player = 0; player < playerCount; player++) {
		controllers[player].leftStick = platformInput->GetGamepadStick(true, player);
		controllers[player].rightStick = platformInput->GetGamepadStick(false, player);

		for(uint32_t button = 0; button < controllers[player].numBinds; button++) {
			controllers[player].buttons[button].pressedLastFrame = controllers[player].buttons[button].pressed;
			controllers[player].buttons[button].pressed = false;
			for(char key : controllers[player].bindings[button].keys) {
				if(platformInput->IsKeyPressed(key, player)) {
					controllers[player].buttons[button].pressed = true;
					break;
				}
			}
			if(controllers[player].buttons[button].pressed) continue;
			for(GamepadButton gamepadButton : controllers[player].bindings[button].buttons) {
				if(platformInput->IsButtonPressed(gamepadButton, player)) {
					controllers[player].buttons[button].pressed = true;
					break;
				}
			}
		}
	}

	platformInput->ClearInputs();
}

void InputManager::SetRumble(uint32_t playerIndex, float strength0to1) {
	platformInput->SetRumble(playerIndex, strength0to1);
}

bool InputManager::WasMouseClicked(bool left) const {
	return (left ? mouse.leftClicked && !mouse.lastFrame.leftClicked : mouse.rightClicked && !mouse.lastFrame.rightClicked);
}

bool InputManager::WasMouseReleased(bool left) const {
	return (left ? !mouse.leftClicked && mouse.lastFrame.leftClicked : !mouse.rightClicked && mouse.lastFrame.rightClicked);
}

Vector2 InputManager::GetMouseWorldPosition(const Camera* camera) const {
	Vector2 screenCenter = *camera->worldMatrix * Vector2::Zero;
	Vector2 fromCenter = (mouse.screenPos * camera->GetWorldDimensions() * 0.5f).Rotate(camera->transform->rotation);
	return screenCenter + fromCenter;
}

Vector2 InputManager::GetMouseWorldDisplacement(const Camera* camera) const {
	Vector2 newPos = (mouse.screenPos * camera->GetWorldDimensions() * 0.5f).Rotate(camera->transform->rotation);
	Vector2 oldPos = (mouse.lastFrame.screenPos * camera->GetWorldDimensions() * 0.5f).Rotate(camera->transform->rotation);
	return newPos - oldPos;
}
