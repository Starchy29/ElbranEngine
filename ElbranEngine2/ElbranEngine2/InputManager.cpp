#include "InputManager.h"
#include "Scene.h"
#include "Math.h"
#include <cassert>

InputManager::InputManager() {
	memset(&lastState, 0, sizeof(InputState) * MAX_PLAYERS);
	memset(&currentState, 0, sizeof(InputState) * MAX_PLAYERS);
	memset(&rumbleDurations, 0, sizeof(float) * MAX_PLAYERS);
	memset(&bindings, 0, sizeof(KeyBinds) * (int)InputAction::ACTION_COUNT * MAX_PLAYERS);
	mouseWheelDelta = 0.f;
	for(int i = 0; i < MAX_PLAYERS; i++) {
		bindings[i][(int)InputAction::Up].keys[0] = 'W';
		bindings[i][(int)InputAction::Up].keys[1] = KEY_UP_ARROW;
		bindings[i][(int)InputAction::Up].buttons[0] = GamepadButton::LStickUp;
		bindings[i][(int)InputAction::Up].buttons[1] = GamepadButton::DpadUp;

		bindings[i][(int)InputAction::Down].keys[0] = 'S';
		bindings[i][(int)InputAction::Down].keys[1] = KEY_DOWN_ARROW;
		bindings[i][(int)InputAction::Down].buttons[0] = GamepadButton::LStickDown;
		bindings[i][(int)InputAction::Down].buttons[1] = GamepadButton::DpadDown;

		bindings[i][(int)InputAction::Left].keys[0] = 'A';
		bindings[i][(int)InputAction::Left].keys[1] = KEY_LEFT_ARROW;
		bindings[i][(int)InputAction::Left].buttons[0] = GamepadButton::LStickLeft;
		bindings[i][(int)InputAction::Left].buttons[1] = GamepadButton::DpadLeft;

		bindings[i][(int)InputAction::Right].keys[0] = 'D';
		bindings[i][(int)InputAction::Right].keys[1] = KEY_RIGHT_ARROW;
		bindings[i][(int)InputAction::Right].buttons[0] = GamepadButton::LStickRight;
		bindings[i][(int)InputAction::Right].buttons[1] = GamepadButton::DpadRight;

		bindings[i][(int)InputAction::Select].keys[0] = KEY_MOUSE_LEFT;
		bindings[i][(int)InputAction::Select].keys[1] = KEY_ENTER;
		bindings[i][(int)InputAction::Select].buttons[0] = GamepadButton::A;

		bindings[i][(int)InputAction::Back].keys[0] = KEY_BACKSPACE;
		bindings[i][(int)InputAction::Back].keys[1] = KEY_ESCAPE;
		bindings[i][(int)InputAction::Back].buttons[0] = GamepadButton::B;
	}
}

void InputManager::Update(float deltaTime) {
	memcpy(&lastState, &currentState, sizeof(InputState) * MAX_PLAYERS);
	CheckInputs();
	mouseWheelDelta = DetermineMouseSpin();

	for(int player = 0; player < MAX_PLAYERS; player++) {
		if(rumbleDurations[player] > 0.f) {
			rumbleDurations[player] -= deltaTime;
			if(rumbleDurations[player] <= 0.f) {
				SetRumble(player, 0.f);
			}
		}

		currentState[player].leftStick = GetGamepadStick(true, player);
		currentState[player].rightStick = GetGamepadStick(false, player);

		for(int action = 0; action < (int)InputAction::ACTION_COUNT; action++) {
			currentState[player].buttons[action] = false;
			for(char key : bindings[player][action].keys) {
				if(IsKeyPressed(key, player)) {
					currentState[player].buttons[action] = true;
					break;
				}
			}
			if(currentState[player].buttons[action]) {
				continue;
			}
			for(GamepadButton button : bindings[player][action].buttons) {
				if(IsButtonPressed(button, player)) {
					currentState[player].buttons[action] = true;
					break;
				}
			}
		}
	}

	ClearInputs();
}

bool InputManager::IsPressed(InputAction action, int playerIndex) {
	return currentState[playerIndex].buttons[(int)action];
}

bool InputManager::JustPressed(InputAction action, int playerIndex) {
	return currentState[playerIndex].buttons[(int)action] && !lastState[playerIndex].buttons[(int)action];
}

bool InputManager::JustReleased(InputAction action, int playerIndex) {
	return !currentState[playerIndex].buttons[(int)action] && lastState[playerIndex].buttons[(int)action];
}

Vector2 InputManager::GetStick(bool left, int playerIndex) {
	return left ? currentState[playerIndex].leftStick : currentState[playerIndex].rightStick;
}

void InputManager::Rumble(int playerIndex, float strength0to1, float duration) {
	assert(duration > 0.f && "attempted to rumble a controller for a non-positive duration");
	SetRumble(playerIndex, strength0to1);
	rumbleDurations[playerIndex] = duration;
}

Vector2 InputManager::GetMousePosition(const Camera* camera) {
	Vector2 screenCenter = *camera->worldMatrix * Vector2::Zero;
	Vector2 fromCenter = (GetMouseScreenPosition() * camera->GetWorldDimensions() * 0.5f).Rotate(camera->transform->rotation);
	return screenCenter + fromCenter;
}

float InputManager::GetMouseWheelSpin() {
	return mouseWheelDelta;
}
