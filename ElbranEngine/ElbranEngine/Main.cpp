// https://learn.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-170

#include <Windows.h>
#include <tchar.h>
#include <memory>
#include "DXGame.h"

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

	HRESULT hRes = DXGame::Initialize(hInstance);
	if (FAILED(hRes)) return hRes;

	return GameInstance->Run();
}