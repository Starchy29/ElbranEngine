#ifdef WINDOWS
#include <Windows.h>
#include <tchar.h>
#include <memory>
#include "Application.h"
#include "DirectXAPI.h"
#include "Configs.h"

HWND windowHandle;
__int64 lastPerfCount;
double perfCountSecs;
double minSecsPerFrame; // inverse of max fps
DirectXAPI* directX;

void RunApp() {
	// main message loop
	bool hasMsg;
	MSG msg = {};
	while(msg.message != WM_QUIT) {
		hasMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		if(hasMsg) { // only update game when not processing a message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			__int64 currentCount;
			QueryPerformanceCounter((LARGE_INTEGER*) &currentCount);
			double deltaTime = (currentCount - lastPerfCount) * perfCountSecs;
			if(deltaTime < minSecsPerFrame) {
				continue;
			}
			lastPerfCount = currentCount;

			float fDeltaTime = (float)deltaTime;

			app->Update(fDeltaTime);

			#if defined(DEBUG) | defined(_DEBUG)
			/*if (SHOW_FPS) {
				timeSinceFPSUpdate += fDeltaTime;
				if(timeSinceFPSUpdate >= 0.5) {
					timeSinceFPSUpdate = 0.0f;
					SetWindowText(windowHandle, (_T(GAME_TITLE) + std::wstring(L" FPS: ") + std::to_wstring(1.0 / deltaTime)).c_str());
				}
			}*/
			#endif
		}
	}
}

// global callback that processes messages from the operating system
LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MENUCHAR:
		// prevent beeping from alt-enter
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		// set minimum window sizes
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			// dont update when minimized
			return 0;

		// Save the new client area dimensions.
		//windowWidth = LOWORD(lParam);
		//windowHeight = HIWORD(lParam);

		/*if (dxCore) {
			directX->Resize(DirectX::XMINT2(windowWidth, windowHeight), viewAspectRatio);
		}*/

		return 0;
	case WM_MOUSEWHEEL:
		//input->mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void InitWindow(HINSTANCE hInstance) {
	static TCHAR szWindowClass[] = _T("ElbranEngineWindow");
	static TCHAR szTitle[] = _T("Elbran Engine");

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;

	// make an icon resource, then find its code in resource.h. 
	//HICON icon;
	// = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	//wcex.hIcon = icon;
	//wcex.hIconSm = icon;

	if(!RegisterClassEx(&wcex)) {
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL
		);
		return;
	}

	int barHeight = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER);
	windowHandle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, // start position
		START_WINDOW_WIDTH, START_WINDOW_HEIGHT + barHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if(!windowHandle) {
		MessageBox(NULL,
			_T("Call to CreateWindowEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
		return;
	}

	// display the window
	ShowWindow(windowHandle, SW_SHOW);
	UpdateWindow(windowHandle);
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
	// set timing data
	minSecsPerFrame = 1.0f / MAX_FPS;
	__int64 perfFreq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq);
	perfCountSecs = 1.0 / (double)perfFreq;
	QueryPerformanceCounter((LARGE_INTEGER*)&lastPerfCount);

	InitWindow(hInstance);
	directX = new DirectXAPI(windowHandle, Int2(START_WINDOW_WIDTH, START_WINDOW_HEIGHT), ASPECT_RATIO);
	app = new Application(directX);
	RunApp();
	delete app;
	return 0;
}
#endif