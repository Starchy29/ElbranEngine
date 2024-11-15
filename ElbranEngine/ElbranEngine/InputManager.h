#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include "Camera.h"

#define Inputs InputManager::GetInstance()
#define VK_MOUSE_LEFT VK_LBUTTON
#define VK_MOUSE_RIGHT VK_RBUTTON
#define VK_MOUSE_MID VK_MBUTTON

class InputManager
{
	friend class DXGame;

public:
	static void Initialize(HWND windowHandle);
	static InputManager* GetInstance();

	InputManager(HWND windowHandle);
	~InputManager();

	void Update();

	bool IsPressed(int key);
	bool JustPressed(int key);
	bool JustReleased(int key);

	Vector2 GetStick(bool left, int slot = 0);
	Vector2 GetMousePosition(Camera* worldView);
	float GetMouseWheelSpin();

private:
	static InputManager* instance;

	HWND windowHandle;
	PBYTE keyboardState;
	PBYTE previousKeyboard;
	DirectX::XMFLOAT2 mouseScreenPos;
	float mouseWheelDelta;
	Vector2 gamepadLeftSticks[4];
	Vector2 gamepadRightSticks[4];
};

