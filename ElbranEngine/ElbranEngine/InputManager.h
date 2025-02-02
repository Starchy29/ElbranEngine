#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <vector>
#include <Xinput.h>
#include "Vector2.h"

class Camera;

#define VK_MOUSE_LEFT VK_LBUTTON
#define VK_MOUSE_RIGHT VK_RBUTTON
#define VK_MOUSE_MID VK_MBUTTON

enum class InputAction {
	Up = 0,
	Down,
	Left,
	Right,
	Select,
	Back,

	ACTION_COUNT // number of elements, must always be last
};

enum class GamepadButton {
	A,
	B,
	X,
	Y,
	LStickUp,
	LStickDown,
	LStickLeft,
	LStickRight,
	RStickUp,
	RStickDown,
	RStickLeft,
	RStickRight,
	DpadUp,
	DpadDown,
	DpadLeft,
	DpadRight,
	LeftTrigger,
	RightTrigger,
	LeftBumper,
	RightBumper,
	LStickPress,
	RStickPress,
	Start,
	Select
};

class InputManager
{
	friend class Application;

public:
	// input is a char represeneting a keyboard or a key starting with prefix VK_
	bool IsKeyPressed(int key);
	bool KeyJustPressed(int key);
	bool KeyJustReleased(int key);

	bool IsButtonPressed(int gamepadIndex, GamepadButton button);
	bool ButtonJustPressed(int gamepadIndex, GamepadButton button);
	bool ButtonJustReleased(int gamepadIndex, GamepadButton button);

	bool IsPressed(InputAction action);
	bool JustPressed(InputAction action);
	bool JustReleased(InputAction action);

	Vector2 GetStick(bool left, int slot = 0);
	Vector2 GetMousePosition(Camera* worldView);
	float GetMouseWheelSpin();
	void SetVibration(int controllerIndex, float strength);

	// prevent copying
	InputManager(const InputManager&) = delete;
	void operator=(const InputManager&) = delete;

private:
	struct GamepadData {
		bool connected;
		XINPUT_GAMEPAD state;
		Vector2 leftStick;
		Vector2 rightStick;
	};

	struct KeyBind {
		std::vector<int> keys;
		std::vector<GamepadButton> buttons;
	};

	HWND windowHandle;

	PBYTE keyboardState;
	PBYTE previousKeyboard;

	DirectX::XMFLOAT2 mouseScreenPos;
	float mouseWheelDelta;

	GamepadData lastGamepads[4];
	GamepadData gamepads[4];

	KeyBind keyBinds[(int)InputAction::ACTION_COUNT];

	InputManager(HWND windowHandle);
	~InputManager();

	void Update();
	bool IsGamepadPressed(const GamepadData& gamepadState, GamepadButton button);
};

