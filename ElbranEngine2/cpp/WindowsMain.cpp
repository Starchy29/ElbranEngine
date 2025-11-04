#ifdef WINDOWS
#include <Windows.h>
#include <memory>
#include "Application.h"
#include "AppPointer.h"
#include "DirectXAPI.h"
#include "WindowsAudio.h"
#include "WindowsInput.h"
#include "Configs.h"
#include "LoadedFile.h"
#include <bit>
#include <string>

HWND windowHandle;
__int64 lastPerfCount;
double perfCountSecs;
double minSecsPerFrame; // inverse of max fps
DirectXAPI* directX;
WindowsInput* input;
std::wstring filePath;

#if defined(DEBUG) | defined(_DEBUG)
float timeSinceFPSUpdate = 0.f;
#endif

void UpdateApp() {
	__int64 currentCount;
	QueryPerformanceCounter((LARGE_INTEGER*) &currentCount);
	double deltaTime = (currentCount - lastPerfCount) * perfCountSecs;
	if(deltaTime < minSecsPerFrame) {
		return;
	}
	lastPerfCount = currentCount;
			
	float fDeltaTime = (float)deltaTime;

	app->Update(fDeltaTime);

	#if defined(DEBUG) | defined(_DEBUG)
	if(SHOW_FPS) {
		timeSinceFPSUpdate += fDeltaTime;
		if(timeSinceFPSUpdate >= 0.5) {
			timeSinceFPSUpdate = 0.0f;
			std::wstring message = L"";
			message = message + GAME_TITLE + L" FPS: " + std::to_wstring(1.0 / deltaTime);
			SetWindowTextW(windowHandle, message.c_str());
		}
	}
	#endif
}

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
			UpdateApp();
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
		if(wParam == SIZE_MINIMIZED) {
			// dont update when minimized
			return 0;
		}

		// Save the new client area dimensions.
		if(directX) {
			directX->Resize(Int2(LOWORD(lParam), HIWORD(lParam)), ASPECT_RATIO);
		}

		return 0;
	case WM_MOUSEWHEEL:
		input->mouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_ENTERSIZEMOVE:
		SetTimer(windowHandle, 1, USER_TIMER_MINIMUM, NULL);
		break;
	case WM_EXITSIZEMOVE:
		KillTimer(windowHandle, 1);
		return 0;
	case WM_TIMER:
		// timer allows the app to update when dragging/resizing the window
		UpdateApp();
		return 0;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	return 0;
}

void InitWindow(HINSTANCE hInstance) {
	static wchar_t szWindowClass[] = L"ElbranEngineWindow";
	static wchar_t szTitle[] = GAME_TITLE;

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
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

	if(!RegisterClassExW(&wcex)) {
		MessageBoxW(NULL,
			L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour",
			NULL
		);
		return;
	}

	int barHeight = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER);
	windowHandle = CreateWindowExW(
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
		MessageBoxW(NULL,
			L"Call to CreateWindowEx failed!",
			L"Windows Desktop Guided Tour",
			NULL);
		return;
	}

	// display the window
	ShowWindow(windowHandle, SW_SHOW);
	UpdateWindow(windowHandle);
}

void QuitApp() {
	PostQuitMessage(0);
}

LoadedFile LoadWindowsFile(std::wstring fileName) {
	HANDLE fileHandle = CreateFileW((filePath + fileName).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if(fileHandle == INVALID_HANDLE_VALUE) return {};
	LoadedFile file = {};

	// determine file size
	LARGE_INTEGER size;
	bool success = GetFileSizeEx(fileHandle, &size);
	if(!success) return {};
	file.fileSize = (uint64_t)size.QuadPart;
	ASSERT(file.fileSize < 0xFFFFFFFF); // windows file read has max size

	// read file
	file.bytes = new uint8_t[file.fileSize];
	DWORD bytesRead;
	success = ReadFile(fileHandle, file.bytes, file.fileSize, &bytesRead, 0);
	CloseHandle(fileHandle);
	if(!success || bytesRead != file.fileSize) {
		delete[] file.bytes;
		return {};
	}
	
	bool platformLittleEndian = std::endian::native == std::endian::little;
	bool platformBigEndian = std::endian::native == std::endian::big;
	return file;
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

	// determine file path
	wchar_t directory[1024] = {};
	GetModuleFileNameW(0, directory, 1024);
	wchar_t* lastSlash = wcsrchr(directory, '\\');
	if(lastSlash) {
		// null terminate here to eliminate .exe name and leave the directory
		*(lastSlash + 1) = 0;
	}
	filePath = directory;

	InitWindow(hInstance);
	RECT windowRect;
	GetClientRect(windowHandle, &windowRect);
	LoadedFile sampleShader = LoadWindowsFile(L"shaders\\CameraVS.cso");
	directX = new DirectXAPI();
	directX->Initialize(windowHandle, Int2(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top), ASPECT_RATIO, &sampleShader);
	sampleShader.Release();
	input = new WindowsInput(windowHandle);
	app = (Application*)calloc(1, sizeof(Application));
	app->Initialize(std::endian::native == std::endian::little, LoadWindowsFile, directX, new WindowsAudio(), input);
	app->quitFunction = QuitApp;
	RunApp();
	app->Release();
	free(app);
	return 0;
}
#endif