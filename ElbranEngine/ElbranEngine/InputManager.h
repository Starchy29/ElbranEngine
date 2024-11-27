#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include "Camera.h"

#define VK_MOUSE_LEFT VK_LBUTTON
#define VK_MOUSE_RIGHT VK_RBUTTON
#define VK_MOUSE_MID VK_MBUTTON

class InputManager
{
	friend class Application;

public:
	bool IsPressed(int key);
	bool JustPressed(int key);
	bool JustReleased(int key);

	DirectX::SimpleMath::Vector2 GetStick(bool left, int slot = 0);
	DirectX::SimpleMath::Vector2 GetMousePosition(Camera* worldView);
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
	DirectX::SimpleMath::Vector2 gamepadLeftSticks[4];
	DirectX::SimpleMath::Vector2 gamepadRightSticks[4];

	InputManager(HWND windowHandle);
	~InputManager();

	void Update();
};

