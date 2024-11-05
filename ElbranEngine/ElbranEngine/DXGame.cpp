#pragma comment(lib,"d3d11.lib")

#include "DXGame.h"
#include <tchar.h>
#include "Color.h"
#include <d3dcompiler.h>
#include "Vertex.h"
#include "AssetManager.h"
#include "WICTextureLoader.h"
#include "InputManager.h"

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

	hRes = instance->InitDirectX();
	if(FAILED(hRes)) return hRes;

	hRes = instance->LoadAssets();
	if(FAILED(hRes)) return hRes;

	Inputs->Initialize(instance->windowHandle);

	return S_OK;
}

DXGame::~DXGame() {
	delete sampleScene;
}

HRESULT DXGame::Run() {
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
			double deltaTime = (currentCount - lastPerfCount) * performanceCountSeconds;
			if(deltaTime < 0.0) {
				deltaTime = 0.0;
			}
			lastPerfCount = currentCount;

			Inputs->Update();
			Update(deltaTime);
			Render();
		}
	}

	return (int)msg.wParam;
}

LRESULT DXGame::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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

		RECT windowRect;
		GetWindowRect(windowHandle, &windowRect);
		//windowDims.x = windowRect.right - windowRect.left;
		//windowDims.y = windowRect.bottom - windowRect.top;

		// update directX
		if(dxDevice) {
			Resize();
		}

		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

float DXGame::GetAspectRatio() {
	return aspectRatio;
}

DirectX::XMINT2 DXGame::GetWindowDims() {
	return windowDims;
}

DirectX::XMINT2 DXGame::GetViewportDims() {
	return viewportDims;
}

DirectX::XMINT2 DXGame::GetViewportShift() {
	return viewportShift;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> DXGame::GetSamplerState() {
	return samplerState;
}

void DXGame::LoadTexture(std::wstring fileName, ID3D11ShaderResourceView** destination) {
	std::wstring fullPath = exePath + L"Assets\\" + fileName;
	CreateWICTextureFromFile(dxDevice.Get(), dxContext.Get(), fullPath.c_str(), nullptr, destination);
}

HRESULT DXGame::LoadAssets() {
	Assets->cameraVS = std::make_shared<VertexShader>(dxDevice, dxContext, L"CameraVS.cso");
	Assets->imagePS = std::make_shared<PixelShader>(dxDevice, dxContext, L"TexturePS.cso");
	Assets->colorPS = std::make_shared<PixelShader>(dxDevice, dxContext, L"ColorFillPS.cso");

	// create default sampler state
	D3D11_SAMPLER_DESC samplerDescription = {};
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//samplerDescription.MaxAnisotropy = 8;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	dxDevice.Get()->CreateSamplerState(&samplerDescription, samplerState.GetAddressOf());

	// create unit square
	Vertex vertices[] = {
		{ DirectX::XMFLOAT2(-0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f) },
		{ DirectX::XMFLOAT2(-0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 0.0f) },
		{ DirectX::XMFLOAT2(0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f) }
	};

	unsigned int indices[] = {
		0, 1, 3, // clockwise winding order
		1, 2, 3
	};

	Assets->unitSquare = std::make_shared<Mesh>(dxDevice, dxContext, &vertices[0], 4, &indices[0], 6);

	// sample assets
	LoadTexture(L"temp sprite.png", Assets->testImage.GetAddressOf());
	sampleScene = new Scene(100);
	
	DirectX::XMFLOAT2 dims = sampleScene->GetCamera()->GetWorldDimensions();

	testObject = new GameObject(Color::Cyan);
	sampleScene->AddObject(testObject);
	//testObject->colorTint = Color::Red;
	//testObject->GetTransform()->SetScale(dims.x + 3, dims.y + 3);
	//testObject->GetTransform()->SetZ(1);
	//testObject->GetTransform()->SetPosition(DirectX::XMFLOAT2(-10.0f, 0.0f));

	return S_OK;
}

void DXGame::Update(float deltaTime) {
	//testObject->colorTint.blue += 0.0008f;
	/*if(Input->JustReleased(VK_MOUSE_LEFT)) {
		testObject->GetTransform()->TranslateAbsolute(5.0f, 0.0f);
	}*/
	testObject->GetTransform()->SetPosition(Inputs->GetMousePosition(sampleScene->GetCamera()));
}

void DXGame::Draw() {
	sampleScene->Draw();
}

void DXGame::Render() { 
	// erase screen and depth buffer
	dxContext->ClearRenderTargetView(backBufferView.Get(), Color::Black);
	dxContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Draw();

	swapChain->Present(0, 0);
	dxContext->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());
}

HRESULT DXGame::InitWindow() {
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("Elbran Engine");

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
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		deviceFlags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&context
	);

	if(FAILED(hr)) {
		return hr;
	}

	device.As(&dxDevice);
	context.As(&dxContext);

	// create swap chain
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.Windowed = TRUE; // initial full-screen state
	desc.BufferCount = 2;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;      // multisampling setting
	desc.SampleDesc.Quality = 0;    // vendor-specific flag
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.OutputWindow = windowHandle;

	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	dxDevice.As(&dxgiDevice);
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
		static_cast<UINT> (windowDims.x),
		static_cast<UINT> (windowDims.x),
		1,
		1,
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

	// create viewport
	Resize();

	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

void DXGame::Resize() {
	backBufferView.Reset();
	depthStencilView.Reset();

	swapChain->ResizeBuffers(2, windowDims.x, windowDims.y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// recreate back buffer
	ID3D11Texture2D* backBufferTexture = 0;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
	if(backBufferTexture) {
		dxDevice->CreateRenderTargetView(
			backBufferTexture,
			0,
			backBufferView.ReleaseAndGetAddressOf()
		);
		backBufferTexture->Release();
	}

	// recreate depth stencil
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = windowDims.x;
	depthStencilDesc.Height = windowDims.y;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D* depthBufferTexture = 0;
	dxDevice->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	if(depthBufferTexture != 0) {
		dxDevice->CreateDepthStencilView(
			depthBufferTexture,
			0,
			depthStencilView.ReleaseAndGetAddressOf()
		);
		depthBufferTexture->Release();
	}

	// bind to the pipeline
	dxContext->OMSetRenderTargets(
		1,
		backBufferView.GetAddressOf(),
		depthStencilView.Get()
	);

	// set viewport
	float windowAspectRatio = (float)windowDims.x / windowDims.y;
	viewportShift = DirectX::XMINT2(0, 0);
	viewportDims = windowDims;
	if(windowAspectRatio > aspectRatio) {
		viewportDims.x = windowDims.y * aspectRatio;
		viewportShift.x = (windowDims.x - viewportDims.x) / 2;
	} else {
		viewportDims.y = windowDims.x / aspectRatio;
		viewportShift.y = (windowDims.y - viewportDims.y) / 2;
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewportShift.x;
	viewport.TopLeftY = viewportShift.y;
	viewport.Width = (float)viewportDims.x;
	viewport.Height = (float)viewportDims.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dxContext->RSSetViewports(1, &viewport);
}

DXGame::DXGame(HINSTANCE hInst) {
	hInstance = hInst;
	aspectRatio = 16.0f / 9.0f;
	int windowWidth = 960;
	windowDims = DirectX::XMINT2(windowWidth, windowWidth / aspectRatio);

	// set up timing info
	__int64 perfFreq;
	QueryPerformanceFrequency((LARGE_INTEGER*) &perfFreq);
	performanceCountSeconds = 1.0 / (double)perfFreq;
	
	QueryPerformanceCounter((LARGE_INTEGER*) &lastPerfCount);

	// determine file path
	wchar_t directory[1024] = {};
	GetModuleFileNameW(0, directory, 1024);
	wchar_t* lastSlash = wcsrchr(directory, '\\');
	if(lastSlash) {
		// null terminate here to eliminate .exe name and leave directory
		*(lastSlash+1) = 0;
	}
	exePath = directory;
}
