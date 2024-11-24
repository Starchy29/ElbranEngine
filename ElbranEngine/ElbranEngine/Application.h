#pragma once
#include <Windows.h>
#include "DXCore.h"
#include "NewGame.h"
#include "InputManager.h"
#include "AssetManager.h"

#define APP Application::GetInstance()

// singleton container for all core components of the game/app
class Application
{
public:
	static HRESULT Initialize(HINSTANCE hInst, WNDPROC procCallback, Application** singletonPtr);
	static const Application* GetInstance();

	HRESULT Run();
	LRESULT ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Dispose();

	DXCore* GetDXCore() const;
	const NewGame* GetGame() const;
	InputManager* Input() const;
	const AssetManager* Assets() const;

	float GetViewAspectRatio() const;
	std::wstring ExePath() const;

	// prevent copying
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

private:
	static Application* instance;
	std::wstring exePath;

	HINSTANCE hInstance;
	HWND windowHandle;

	float viewAspectRatio;
	DirectX::XMINT2 windowDims;

	double minSecsPerFrame; // inverse of max fps
	double perfCountSecs;
	__int64 lastPerfCount;

	DXCore* dxCore;
	NewGame* game;
	InputManager* input;
	AssetManager* assets;
	// sounds

	Application(HINSTANCE hInst);
	~Application();

	HRESULT InitApp(WNDPROC procCallback);
	HRESULT InitWindow(WNDPROC procCallback);

#if defined(DEBUG) | defined(_DEBUG)
	inline void CreateDebugConsole();
#endif
};

