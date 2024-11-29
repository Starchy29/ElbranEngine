#include "Application.h"
#include <tchar.h>

Application* Application::instance;

HRESULT Application::Initialize(HINSTANCE hInst, WNDPROC procCallback, Application** singletonPtr) {
	instance = new Application(hInst);
	*singletonPtr = instance;
	return instance->InitApp(procCallback);
}

HRESULT Application::Run() {
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

			input->Update();
			game->Update(deltaTime);
			dxCore->Render(game);

			input->mouseWheelDelta = 0.0f;
		}
	}

	return (int)msg.wParam;
}

LRESULT Application::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
		windowDims.x = LOWORD(lParam);
		windowDims.y = HIWORD(lParam);

		if(dxCore) {
			dxCore->Resize(windowDims, viewAspectRatio);
		}

		return 0;
	case WM_MOUSEWHEEL:
		input->mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void Application::Dispose() {
	delete instance;
}

DXCore* Application::Graphics() const {
	return dxCore;
}

const Game* Application::GetGame() const {
	return game;
}

InputManager* Application::Input() const {
	return input;
}

const AssetManager* Application::Assets() const {
	return assets;
}

float Application::GetViewAspectRatio() const {
	return viewAspectRatio;
}

std::wstring Application::ExePath() const {
	return exePath;
}

Application::Application(HINSTANCE hInst) {
	hInstance = hInst;

	// window size
	viewAspectRatio = 16.0f / 9.0f;
	windowDims.x = 960;
	windowDims.y = (float)windowDims.x / viewAspectRatio;

	// set timing data
	minSecsPerFrame = 1.0f / 60.0f;
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
	exePath = directory;

#if defined(DEBUG) | defined(_DEBUG)
	CreateDebugConsole();
#endif
}

Application::~Application() {
	delete dxCore;
	delete game;
	delete input;
	delete assets;
}

HRESULT Application::InitApp(WNDPROC procCallback) {
	HRESULT result = InitWindow(procCallback);
	if (FAILED(result)) return result;

	dxCore = new DXCore(windowHandle, windowDims, viewAspectRatio, &result);
	if (FAILED(result)) return result;

	input = new InputManager(windowHandle);
	assets = new AssetManager();
	game = new Game(assets);
	return S_OK;
}

HRESULT Application::InitWindow(WNDPROC procCallback) {
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("Elbran Engine");

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = procCallback;
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

	windowHandle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, // start position
		windowDims.x, windowDims.y,
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
		return 1;
	}

	// display the window
	ShowWindow(windowHandle, SW_SHOW);
	UpdateWindow(windowHandle);

	return S_OK;
}

const Application* Application::GetInstance() {
	return instance;
}

#if defined(DEBUG) | defined(_DEBUG)
inline void Application::CreateDebugConsole() {
	CONSOLE_SCREEN_BUFFER_INFO consoleinfo;
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleinfo);
	consoleinfo.dwSize.Y = 50;
	consoleinfo.dwSize.X = 100;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleinfo.dwSize);

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = 50;
	rect.Bottom = 12;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

	FILE* stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
}
#endif