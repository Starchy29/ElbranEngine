#include "InputManager.h"
#include "Scene.h"
#include "Math.h"

InputManager::InputManager() {
	memset(&lastState, 0, sizeof(InputState) * MAX_PLAYERS);
	memset(&currentState, 0, sizeof(InputState) * MAX_PLAYERS);
	memset(&rumbleDurations, 0, sizeof(float) * MAX_PLAYERS);
	memset(&bindings, 0, sizeof(KeyBinds) * (int)InputAction::ACTION_COUNT * MAX_PLAYERS);
	mouseWheelDelta = 0.f;
	mouseScreenPos = Vector2::Zero;
	lastMousePos = Vector2::Zero;
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

		bindings[i][(int)InputAction::LeftCLick].keys[0] = KEY_MOUSE_LEFT;
		bindings[i][(int)InputAction::RightClick].keys[0] = KEY_MOUSE_RIGHT;
	}
}

void InputManager::Update(float deltaTime) {
	memcpy(&lastState, &currentState, sizeof(InputState) * MAX_PLAYERS);
	lastMousePos = mouseScreenPos;
	CheckInputs();
	mouseScreenPos = GetMouseScreenPosition();
	mouseWheelDelta = DetermineMouseSpin();

	for(uint8_t player = 0; player < MAX_PLAYERS; player++) {
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

bool InputManager::IsPressed(InputAction action, uint8_t playerIndex) {
	return currentState[playerIndex].buttons[(int)action];
}

bool InputManager::JustPressed(InputAction action, uint8_t playerIndex) {
	return currentState[playerIndex].buttons[(int)action] && !lastState[playerIndex].buttons[(int)action];
}

bool InputManager::JustReleased(InputAction action, uint8_t playerIndex) {
	return !currentState[playerIndex].buttons[(int)action] && lastState[playerIndex].buttons[(int)action];
}

Vector2 InputManager::GetStick(bool left, uint8_t playerIndex) {
	return left ? currentState[playerIndex].leftStick : currentState[playerIndex].rightStick;
}

void InputManager::Rumble(uint8_t playerIndex, float strength0to1, float duration) {
	ASSERT(duration > 0.f);
	SetRumble(playerIndex, strength0to1);
	rumbleDurations[playerIndex] = duration;
}

Vector2 InputManager::GetMousePosition(const Camera* camera) {
	Vector2 screenCenter = *camera->worldMatrix * Vector2::Zero;
	Vector2 fromCenter = (mouseScreenPos * camera->GetWorldDimensions() * 0.5f).Rotate(camera->transform->rotation);
	return screenCenter + fromCenter;
}

Vector2 InputManager::GetMouseDelta(const Camera* camera) {
	Vector2 newPos = (mouseScreenPos * camera->GetWorldDimensions() * 0.5f).Rotate(camera->transform->rotation);
	Vector2 oldPos = (lastMousePos * camera->GetWorldDimensions() * 0.5f).Rotate(camera->transform->rotation);
	return newPos - oldPos;
}

float InputManager::GetMouseWheelSpin() {
	return mouseWheelDelta;
}
