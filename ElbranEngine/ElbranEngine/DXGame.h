// https://learn.microsoft.com/en-us/windows/win32/direct3dgetstarted/building-your-first-directx-app
#pragma once
#include <Windows.h>

#define GameInstance DXGame::GetInstance()

class DXGame
{
public:
	static DXGame* GetInstance();
	static HRESULT Initialize(HINSTANCE hInst);

	HRESULT Run();
	LRESULT ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static DXGame* instance;

	HINSTANCE hInstance;

	HRESULT InitWindow();
	//HRESULT InitDirectX();

	DXGame(HINSTANCE hInst);
};

