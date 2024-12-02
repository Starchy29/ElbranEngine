#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include "Camera.h"
#include <vector>

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

class InputManager
{
	friend class Application;

public:
	bool IsPressed(int key);
	bool JustPressed(int key);
	bool JustReleased(int key);

	bool IsPressed(InputAction action);
	bool JustPressed(InputAction action);
	bool JustReleased(InputAction action);

	Vector2 GetStick(bool left, int slot = 0);
	Vector2 GetMousePosition(Camera* worldView);
	float GetMouseWheelSpin();

	// prevent copying
	InputManager(const InputManager&) = delete;
	void operator=(const InputManager&) = delete;

private:
	HWND windowHandle;
	PBYTE keyboardState;
	PBYTE previousKeyboard;
	DirectX::XMFLOAT2 mouseScreenPos;
	float mouseWheelDelta;
	Vector2 gamepadLeftSticks[4];
	Vector2 gamepadRightSticks[4];
	std::vector<int> actionKeys[(int)InputAction::ACTION_COUNT];

	InputManager(HWND windowHandle);
	~InputManager();

	void Update();
};

