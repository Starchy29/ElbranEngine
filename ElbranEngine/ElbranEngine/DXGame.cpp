#include "DXGame.h"
#include <tchar.h>

// global callback that processes messages from the operating system
LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	return GameInstance->ProcessMessage(hWnd, message, wParam, lParam);
}

DXGame* DXGame::instance;

DXGame* DXGame::GetInstance() {
	return instance;
}

HRESULT DXGame::Initialize(HINSTANCE hInst) {
	instance = new DXGame(hInst);

	HRESULT hRes = instance->InitWindow();
	if(FAILED(hRes)) return hRes;

	return S_OK;
}

HRESULT DXGame::Run() {
	// main message loop
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT DXGame::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, Windows desktop!");

	switch (message) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Here your application is laid out.
		// For this introduction, we just print out "Hello, Windows desktop!"
		// in the top left corner.
		TextOut(hdc,
			5, 5,
			greeting, _tcslen(greeting));
		// End application specific layout section.

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

HRESULT DXGame::InitWindow() {
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if(!RegisterClassEx(&wcex)) {
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL
		);
		return 1;
	}

	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, // start position
		500, 100, // size
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if(!hWnd) {
		MessageBox(NULL,
			_T("Call to CreateWindowEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
		return 1;
	}

	// display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

DXGame::DXGame(HINSTANCE hInst) {
	hInstance = hInst;
}
