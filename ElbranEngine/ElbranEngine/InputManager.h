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
public:
	static void Initialize(HWND windowHandle);
	static InputManager* GetInstance();

	InputManager(HWND windowHandle);
	~InputManager();

	void Update();

	bool IsPressed(int key);
	bool JustPressed(int key);
	bool JustReleased(int key);

	DirectX::XMFLOAT2 GetMousePosition(Camera* worldView);
	// wheel spin

	// gamepad stick vectors (left, right)

private:
	static InputManager* instance;

	HWND windowHandle;
	PBYTE keyboardState;
	PBYTE previousKeyboard;
	DirectX::XMFLOAT2 mouseScreenPos;
};

