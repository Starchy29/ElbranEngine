#pragma comment(lib,"d3d11.lib")
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
	bool hasMsg;
	MSG msg = {};
	while(msg.message != WM_QUIT) {
		hasMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		if(hasMsg) {
			// process the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// only update game when not processing a message

			// update

			// draw
		}
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

	hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, // start position
		windowWidth, windowHeight,
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

	return S_OK;
}

HRESULT DXGame::InitDirectX() {
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // required for Direct2D compatibility

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// create device and context
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,                    // Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
		0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		deviceFlags,                // Set debug and Direct2D compatibility flags.
		levels,                     // List of feature levels this app can support.
		ARRAYSIZE(levels),          // Size of the list above.
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,                    // Returns the Direct3D device created.
		&featureLevel,				// Returns feature level of device created.
		&context                    // Returns the device immediate context.
	);

	if(FAILED(hr)) {
		return hr;
	}

	device.As(&dxDevice);
	device.As(&dxContext);

	// create swap chain
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
	desc.BufferCount = 2;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;      //multisampling setting
	desc.SampleDesc.Quality = 0;    //vendor-specific flag
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.OutputWindow = hWnd;

	Microsoft::WRL::ComPtr<IDXGIDevice3> giDevice;
	dxgiDevice.As(&giDevice);
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;

	hr = dxgiDevice->GetAdapter(&adapter);
	if(FAILED(hr)) {
		return hr;
	}

	adapter->GetParent(IID_PPV_ARGS(&factory));

	hr = factory->CreateSwapChain(
		device.Get(),
		&desc,
		&swapChain
	);

	if(FAILED(hr)) {
		return hr;
	}

	// create render target
	ID3D11Texture2D* backBuffer;
	hr = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&backBuffer
	);

	if(FAILED(hr)) {
		return hr;
	}

	hr = dxDevice->CreateRenderTargetView(
		backBuffer,
		nullptr,
		backBufferView.GetAddressOf()
	);

	if(FAILED(hr)) {
		return hr;
	}
	backBuffer->Release();

	// create depth stencil
	ID3D11Texture2D* depthStencil = 0;

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (windowWidth),
		static_cast<UINT> (windowHeight),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	hr = dxDevice->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depthStencil
	);

	if(FAILED(hr)) {
		return hr;
	}

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	hr = dxDevice->CreateDepthStencilView(
		depthStencil,
		&depthStencilViewDesc,
		&depthStencilView
	);

	if(FAILED(hr)) {
		return hr;
	}

	depthStencil->Release();

	context->OMSetRenderTargets(
		1,
		backBufferView.GetAddressOf(),
		depthStencilView.Get()
	);

	// create viewport
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)windowWidth;
	viewport.Height = (float)windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	return S_OK;
}

DXGame::DXGame(HINSTANCE hInst) {
	hInstance = hInst;
	windowWidth = 960;
	windowHeight = 540;
}
