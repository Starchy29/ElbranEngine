#ifdef WINDOWS
#include "WindowsInput.h"
#include "Math.h"
#include <Windows.h>
#include <Xinput.h>

#define STICK_MAX 32767.0f
#define GAMEPAD_DEAD_ZONE 0.2f
#define MAX_VIBRATION 65535.0f

WindowsInput::WindowsInput(HWND windowHandle) {
    this->windowHandle = windowHandle;
    gamepadStates = new XINPUT_GAMEPAD[4];
}

WindowsInput::~WindowsInput() {
    delete[] gamepadStates;
}

void WindowsInput::CheckInputs() {
    // get keyboard
    (void)GetKeyboardState(keyboard);

    // get gamepads
    DWORD result;
	for(DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		XINPUT_STATE gamepadData = {};
		result = XInputGetState(i, &gamepadData);
		gamepads[i].connected = (result == ERROR_SUCCESS);

		if(!gamepads[i].connected) {
			continue;
		}

		// save the gamepad state
		gamepadStates[i] = gamepadData.Gamepad;

		// determine stick positions
		short x = gamepadStates[i].sThumbLX;
		short y = gamepadStates[i].sThumbLY;
		if(x < 0) {
			x++; // range is -32768 to 32767
		}
		if(y < 0) {
			y++;
		}
		gamepads[i].leftStick = Vector2(x / STICK_MAX, y / STICK_MAX);

		x = gamepadStates[i].sThumbRX;
		y = gamepadStates[i].sThumbRY;
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

void WindowsInput::ClearInputs() {
    mouseWheelDelta = 0.f;
}

float WindowsInput::DetermineMouseSpin() {
    return mouseWheelDelta;
}

bool WindowsInput::IsKeyPressed(char key, uint8_t playerIndex) {
    switch(key) {
    case KEY_NONE:
        return false;
    case KEY_UP_ARROW:
        key = VK_UP;
        break;
    case KEY_DOWN_ARROW:
        key = VK_DOWN;
        break;
    case KEY_LEFT_ARROW:
        key = VK_LEFT;
        break;
    case KEY_RIGHT_ARROW:
        key = VK_RIGHT;
        break;
    case KEY_MOUSE_LEFT:
        key = VK_LBUTTON;
        break;
    case KEY_MOUSE_RIGHT:
        key = VK_RBUTTON;
        break;
    case KEY_MOUSE_MIDDLE:
        key = VK_MBUTTON;
        break;
    case KEY_SHIFT:
        key = VK_SHIFT;
        break;
    case KEY_ENTER:
        key = VK_RETURN;
        break;
    case KEY_BACKSPACE:
        key = VK_BACK;
        break;
    case KEY_CONTROL:
        key = VK_CONTROL;
        break;
    case KEY_ALT:
        key = VK_MENU;
        break;
    case KEY_ESCAPE:
        key = VK_ESCAPE;
        break;
    case ' ':
        key = VK_SPACE;
        break;
    }

    return keyboard[key] & 0x80; // high-order bit is 1 when the key is pressed
}

bool WindowsInput::IsButtonPressed(GamepadButton button, uint8_t playerIndex) {
    if(!gamepads[playerIndex].connected) {
        return false;
    }

    switch(button) {
    case GamepadButton::A:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_A;
    case GamepadButton::B:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_B;
    case GamepadButton::X:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_X;
    case GamepadButton::Y:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_Y;
    case GamepadButton::LStickUp:
        return gamepads[playerIndex].leftStick.y >= SQRT2_DIV2;
    case GamepadButton::LStickDown:
        return gamepads[playerIndex].leftStick.y <= -SQRT2_DIV2;
    case GamepadButton::LStickLeft:
        return gamepads[playerIndex].leftStick.x <= -SQRT2_DIV2;
    case GamepadButton::LStickRight:
        return gamepads[playerIndex].leftStick.x >= SQRT2_DIV2;
    case GamepadButton::RStickUp:
        return gamepads[playerIndex].rightStick.y >= SQRT2_DIV2;
    case GamepadButton::RStickDown:
        return gamepads[playerIndex].rightStick.y <= -SQRT2_DIV2;
    case GamepadButton::RStickLeft:
        return gamepads[playerIndex].rightStick.x <= -SQRT2_DIV2;
    case GamepadButton::RStickRight:
        return gamepads[playerIndex].rightStick.x >= SQRT2_DIV2;
    case GamepadButton::DpadUp:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_DPAD_UP;
    case GamepadButton::DpadDown:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
    case GamepadButton::DpadLeft:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
    case GamepadButton::DpadRight:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
    case GamepadButton::LeftTrigger:
        return gamepadStates[playerIndex].bLeftTrigger > 200; // 255 is max
    case GamepadButton::RightTrigger:
        return gamepadStates[playerIndex].bRightTrigger > 200; // 255 is max
    case GamepadButton::LeftBumper:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
    case GamepadButton::RightBumper:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
    case GamepadButton::LStickPress:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_LEFT_THUMB;;
    case GamepadButton::RStickPress:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
    case GamepadButton::Start:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_START;
    case GamepadButton::Select:
        return gamepadStates[playerIndex].wButtons & XINPUT_GAMEPAD_BACK;
    }

    return false;
}

Vector2 WindowsInput::GetGamepadStick(bool left, uint8_t playerIndex) {
    if(!gamepads[playerIndex].connected) {
        return Vector2::Zero;
    }
    return left ? gamepads[playerIndex].leftStick : gamepads[playerIndex].rightStick;
}

Vector2 WindowsInput::GetMouseScreenPosition(UInt2 viewDimensions, UInt2 viewOffset) {
    // get mouse
    POINT mouseWindowPos;
    GetCursorPos(&mouseWindowPos);
    ScreenToClient(windowHandle, &mouseWindowPos);

    Vector2 mousePosition;
    mouseWindowPos.x -= viewOffset.x;
    mouseWindowPos.y -= viewOffset.y;
    mousePosition.x = (float)mouseWindowPos.x / viewDimensions.x * 2.0f - 1.0f;
    mousePosition.y = (float)mouseWindowPos.y / viewDimensions.y * 2.0f - 1.0f;
    mousePosition.y *= -1.0f;
    return mousePosition;
}

void WindowsInput::SetRumble(uint8_t playerIndex, float strength) {
    if(!gamepads[playerIndex].connected) {
        return;
    }

	XINPUT_VIBRATION vibeState = {};
	vibeState.wRightMotorSpeed = (unsigned short)(Math::Clamp(strength, 0.f, 1.f) * MAX_VIBRATION);
	vibeState.wLeftMotorSpeed = vibeState.wRightMotorSpeed;

	XInputSetState(playerIndex, &vibeState);
}
#endif