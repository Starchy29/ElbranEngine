// https://learn.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-170

#include <Windows.h>
#include <tchar.h>
#include <memory>
#include "Application.h"

Application* app;

// global callback that processes messages from the operating system
LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	return app->ProcessMessage(hWnd, message, wParam, lParam);
}

// entry point for desktop app
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
#if defined(DEBUG) | defined(_DEBUG)
	// enable memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	HRESULT result = Application::Initialize(hInstance, WndProc, &app);
	if(FAILED(result)) return result;
	result = app->Run();
	app->Dispose();
	return result;
}