#include "InputManager.h"
#include "Application.h"
#include "DXCore.h"
#include "Camera.h"
#include "Tween.h"

#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")
using namespace DirectX;

#define STICK_MAX 32767.0f
#define GAMEPAD_DEAD_ZONE 0.2f
#define KEY_COUNT 256
#define KEY_DOWN(keyByte) (keyByte & 0x80) // high-order bit is 1 when the key is pressed
#define MAX_VIBRATION 65535.0f
#define SQRT2_DIV2 0.71f

InputManager::InputManager(HWND windowHandle) {
	this->windowHandle = windowHandle;

	keyboardState = new BYTE[KEY_COUNT];
	previousKeyboard = new BYTE[KEY_COUNT];
	memset(keyboardState, 0, sizeof(BYTE) * KEY_COUNT);
	memset(previousKeyboard, 0, sizeof(BYTE) * KEY_COUNT);

	memset(&gamepads, 0, sizeof(GamepadData) * 4);

	// set keybinds
	keyBinds[(int)InputAction::Up] = { 
		{ VK_UP, 'W' },
		{ GamepadButton::LStickUp, GamepadButton::DpadUp } 
	};
	keyBinds[(int)InputAction::Down] = {
		{ VK_DOWN, 'S' },
		{ GamepadButton::LStickDown, GamepadButton::DpadDown }
	};
	keyBinds[(int)InputAction::Left] = {
		{ VK_LEFT, 'A' },
		{ GamepadButton::LStickLeft, GamepadButton::DpadLeft }
	};
	keyBinds[(int)InputAction::Right] = {
		{ VK_RIGHT, 'D' },
		{ GamepadButton::LStickRight, GamepadButton::DpadRight }
	};
	keyBinds[(int)InputAction::Select] = {
		{ VK_RETURN, VK_SPACE },
		{ GamepadButton::A }
	};
	keyBinds[(int)InputAction::Back] = {
		{ VK_BACK, VK_ESCAPE },
		{ GamepadButton::B }
	};
}

InputManager::~InputManager() {
	delete[] keyboardState;
	delete[] previousKeyboard;
}

void InputManager::Update() {
	// update keyboard
	memcpy(previousKeyboard, keyboardState, sizeof(unsigned char) * KEY_COUNT);
	(void)GetKeyboardState(keyboardState);

	// update mouse
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);

	XMUINT2 viewShift = APP->Graphics()->GetViewOffset();
	mousePos.x -= viewShift.x;
	mousePos.y -= viewShift.y;

	XMUINT2 viewDims = APP->Graphics()->GetViewDimensions();
	mouseScreenPos.x = (float)mousePos.x / viewDims.x * 2.0f - 1.0f;
	mouseScreenPos.y = (float)mousePos.y / viewDims.y * 2.0f - 1.0f;
	mouseScreenPos.y *= -1.0f;

	// update gamepads
	DWORD result;
	for(DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		lastGamepads[i] = gamepads[i];

		XINPUT_STATE gamepadData = {};
		result = XInputGetState(i, &gamepadData);
		gamepads[i].connected = (result == ERROR_SUCCESS);

		if(!gamepads[i].connected) {
			continue;
		}

		// save the gamepad state
		gamepads[i].state = gamepadData.Gamepad;

		// determine stick positions
		short x = gamepads[i].state.sThumbLX;
		short y = gamepads[i].state.sThumbLY;
		if(x < 0) {
			x++; // range is -32768 to 32767
		}
		if(y < 0) {
			y++;
		}
		gamepads[i].leftStick = Vector2(x / STICK_MAX, y / STICK_MAX);

		x = gamepads[i].state.sThumbRX;
		y = gamepads[i].state.sThumbRY;
		if(x < 0) {
			x++; // range is -32768 to 32767
		}
		if(y < 0) {
			y++;
		}
		gamepads[i].rightStick = Vector2(x / STICK_MAX, y / STICK_MAX);

		if(gamepads[i].leftStick.Length() <= GAMEPAD_DEAD_ZONE) {
			gamepads[i].leftStick = Vector2::Zero;
		}
		if(gamepads[i].rightStick.Length() <= GAMEPAD_DEAD_ZONE) {
			gamepads[i].rightStick = Vector2::Zero;
		}
	}
}

bool InputManager::IsGamepadPressed(const GamepadData& gamepadState, GamepadButton button) {
	switch(button) {
	case GamepadButton::A:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_A;
	case GamepadButton::B:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_B;
	case GamepadButton::X:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_X;
	case GamepadButton::Y:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_Y;
	case GamepadButton::LStickUp:
		return gamepadState.leftStick.y >= SQRT2_DIV2;
	case GamepadButton::LStickDown:
		return gamepadState.leftStick.y <= -SQRT2_DIV2;
	case GamepadButton::LStickLeft:
		return gamepadState.leftStick.x <= -SQRT2_DIV2;
	case GamepadButton::LStickRight:
		return gamepadState.leftStick.x >= SQRT2_DIV2;
	case GamepadButton::RStickUp:
		return gamepadState.rightStick.y >= SQRT2_DIV2;
	case GamepadButton::RStickDown:
		return gamepadState.rightStick.y <= -SQRT2_DIV2;
	case GamepadButton::RStickLeft:
		return gamepadState.rightStick.x <= -SQRT2_DIV2;
	case GamepadButton::RStickRight:
		return gamepadState.rightStick.x >= SQRT2_DIV2;
	case GamepadButton::DpadUp:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_DPAD_UP;
	case GamepadButton::DpadDown:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
	case GamepadButton::DpadLeft:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
	case GamepadButton::DpadRight:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
	case GamepadButton::LeftTrigger:
		return gamepadState.state.bLeftTrigger > 200; // 255 is max
	case GamepadButton::RightTrigger:
		return gamepadState.state.bRightTrigger > 200; // 255 is max
	case GamepadButton::LeftBumper:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
	case GamepadButton::RightBumper:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case GamepadButton::LStickPress:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;;
	case GamepadButton::RStickPress:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
	case GamepadButton::Start:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_START;
	case GamepadButton::Select:
		return gamepadState.state.wButtons & XINPUT_GAMEPAD_BACK;
	}

	return false;
}

bool InputManager::IsKeyPressed(int key) {
	return KEY_DOWN(keyboardState[key]);
}

bool InputManager::KeyJustPressed(int key) {
	return KEY_DOWN(keyboardState[key]) && !KEY_DOWN(previousKeyboard[key]);
}

bool InputManager::KeyJustReleased(int key) {
	return !KEY_DOWN(keyboardState[key]) && KEY_DOWN(previousKeyboard[key]);
}

bool InputManager::IsButtonPressed(int gamepadIndex, GamepadButton button) {
	return gamepads[gamepadIndex].connected && IsGamepadPressed(gamepads[gamepadIndex], button);
}

bool InputManager::ButtonJustPressed(int gamepadIndex, GamepadButton button) {
	return gamepads[gamepadIndex].connected && IsGamepadPressed(gamepads[gamepadIndex], button) && !IsGamepadPressed(lastGamepads[gamepadIndex], button);
}

bool InputManager::ButtonJustReleased(int gamepadIndex, GamepadButton button) {
	return gamepads[gamepadIndex].connected && IsGamepadPressed(lastGamepads[gamepadIndex], button) && !IsGamepadPressed(gamepads[gamepadIndex], button);
}

bool InputManager::IsPressed(InputAction action) {
	for(int key : keyBinds[(int)action].keys) {
		if(IsKeyPressed(key)) {
			return true;
		}
	}
	for(GamepadButton button : keyBinds[(int)action].buttons) {
		if(IsButtonPressed(0, button)) {
			return true;
		}
	}
	return false;
}

bool InputManager::JustPressed(InputAction action) {
	bool pressed = false;
	for(int key : keyBinds[(int)action].keys) {
		if(KeyJustPressed(key)) {
			pressed = true;
		}
		else if(IsKeyPressed(key)) {
			// make sure another key is not already held
			return false;
		}
	}
	for(GamepadButton button : keyBinds[(int)action].buttons) {
		if(ButtonJustPressed(0, button)) {
			pressed = true;
		}
		else if(IsButtonPressed(0, button)) {
			// make sure another key is not already held
			return false;
		}
	}
	return pressed;
}

bool InputManager::JustReleased(InputAction action) {
	bool released = false;
	for(int key : keyBinds[(int)action].keys) {
		if(IsKeyPressed(key)) {
			// make sure another key is not still held
			return false;
		}
		else if(KeyJustReleased(key)) {
			released = true;
		}
	}
	for(GamepadButton button : keyBinds[(int)action].buttons) {
		if(IsButtonPressed(0, button)) {
			// make sure another key is not still held
			return false;
		}
		else if(ButtonJustReleased(0, button)) {
			released = true;
		}
	}
	return released;
}

Vector2 InputManager::GetStick(bool left, int slot) {
	if(!gamepads[slot].connected) {
		return Vector2::Zero;
	}

	if(left) {
		return gamepads[slot].leftStick;
	}
	return gamepads[slot].rightStick;
}

Vector2 InputManager::GetMousePosition(Camera* worldView) {
	XMFLOAT2 cameraCenter = worldView->GetPosition();
	XMFLOAT2 worldScale = worldView->GetWorldDimensions();
	
	XMVECTOR shiftFromMid = XMVectorMultiply(XMLoadFloat2(&mouseScreenPos), XMVectorScale(XMLoadFloat2(&worldScale), 0.5f));
	shiftFromMid = XMVector3Rotate(shiftFromMid, XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), worldView->GetRotation()));
	XMVECTOR worldPos = XMVectorAdd(XMLoadFloat2(&cameraCenter), shiftFromMid);

	XMFLOAT2 result;
	XMStoreFloat2(&result, worldPos);
	return Vector2(result);
}

float InputManager::GetMouseWheelSpin() {
	return mouseWheelDelta;
}

void InputManager::SetVibration(int controllerIndex, float strength) {
	XINPUT_STATE state = {};
	DWORD result = XInputGetState(controllerIndex, &state);

	if(result != ERROR_SUCCESS) {
		// fails if controller is not connected
		return;
	}

	XINPUT_VIBRATION vibeState = {};
	vibeState.wRightMotorSpeed = (unsigned short)(Tween::Clamp(strength, 0.f, 1.f) * MAX_VIBRATION);
	vibeState.wLeftMotorSpeed = vibeState.wRightMotorSpeed;

	XInputSetState(controllerIndex, &vibeState);
}
