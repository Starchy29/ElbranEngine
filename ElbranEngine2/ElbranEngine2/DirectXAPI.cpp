#include "DirectXAPI.h"

DirectXAPI::DirectXAPI(HWND windowHandle, Int2 windowDims, float viewAspectRatio) {
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
	D3D_FEATURE_LEVEL featureLevel;
	D3D11CreateDevice(
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
	device.As(&dxgiDevice);
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;

	dxgiDevice->GetAdapter(&adapter);
	adapter->GetParent(IID_PPV_ARGS(&factory));
	factory->CreateSwapChain(device.Get(), &desc, &swapChain);

	// create render target
	ID3D11Texture2D* backBuffer = nullptr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	device->CreateRenderTargetView(backBuffer, nullptr, backBufferView.GetAddressOf());
	backBuffer->Release();

	// create depth stencil
	ID3D11Texture2D* depthStencil = nullptr;
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (windowDims.x),
		static_cast<UINT> (windowDims.x),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
	);

	device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depthStencil
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &depthStencilView);
	depthStencil->Release();

	D3D11_DEPTH_STENCIL_DESC stencilStateDesc = {};
	stencilStateDesc.DepthEnable = true;
	stencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	stencilStateDesc.StencilEnable = false;
	stencilStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	stencilStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	stencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	stencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	stencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&stencilStateDesc, defaultStencil.GetAddressOf());
	context->OMSetDepthStencilState(defaultStencil.Get(), 0);

	// create the alpha blend state
	D3D11_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = false;
	blendDescription.IndependentBlendEnable = false;
	blendDescription.RenderTarget[0].BlendEnable = true;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blendDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	device->CreateBlendState(&blendDescription, alphaBlendState.GetAddressOf());

	// create the additive blend state
	blendDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	device->CreateBlendState(&blendDescription, additiveBlendState.GetAddressOf());

	// create the viewport
	Resize(windowDims, viewAspectRatio);

	// set other defaults
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//spriteBatch = new DirectX::SpriteBatch(context.Get());
}

DirectXAPI::~DirectXAPI() {

}

void DirectXAPI::Resize(Int2 windowDims, float viewAspectRatio) {
	backBufferView.Reset();
	depthStencilView.Reset();
	swapChain->ResizeBuffers(2, windowDims.x, windowDims.y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// recreate back buffer
	ID3D11Texture2D* backBufferTexture = 0;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
	if(backBufferTexture) {
		device->CreateRenderTargetView(backBufferTexture, 0, backBufferView.ReleaseAndGetAddressOf());
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
	device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	if(depthBufferTexture != 0) {
		device->CreateDepthStencilView(depthBufferTexture, 0, depthStencilView.ReleaseAndGetAddressOf());
		depthBufferTexture->Release();
	}

	// bind to the pipeline
	context->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());

	// set viewport
	float windowAspectRatio = (float)windowDims.x / windowDims.y;
	viewportOffset = Int2(0, 0);
	viewportDims = Int2(windowDims.x, windowDims.y);
	if(windowAspectRatio > viewAspectRatio) {
		viewportDims.x = windowDims.y * viewAspectRatio;
		viewportOffset.x = (windowDims.x - viewportDims.x) / 2;
	} else {
		viewportDims.y = windowDims.x / viewAspectRatio;
		viewportOffset.y = (windowDims.y - viewportDims.y) / 2;
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewportOffset.x;
	viewport.TopLeftY = viewportOffset.y;
	viewport.Width = (float)viewportDims.x;
	viewport.Height = (float)viewportDims.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	// resize post process textures
	//ppTex1.Resize(device, windowDims.x, windowDims.y);
	//ppTex2.Resize(device, windowDims.x, windowDims.y);
	//for(int i = 0; i < MAX_POST_PROCESS_TEXTURES; i++) {
	//	ppHelpers[i].Resize(device, windowDims.x, windowDims.y);
	//}
}

void DirectXAPI::SetBlendMode(BlendState mode) {
	switch (mode) {
	case BlendState::None:
		context->OMSetBlendState(NULL, NULL, 0xffffffff);
		break;
	case BlendState::AlphaBlend:
		context->OMSetBlendState(alphaBlendState.Get(), NULL, 0xffffffff);
		break;
	case BlendState::Additive:
		context->OMSetBlendState(additiveBlendState.Get(), NULL, 0xffffffff);
		break;
	}
}

void DirectXAPI::ClearRenderTarget() {
	float black[4] { 0.f, 0.f, 1.f, 0.f };
	context->ClearRenderTargetView(backBufferView.Get(), black);
}

void DirectXAPI::ClearDepthStencil() {
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DirectXAPI::PresentSwapChain() {
	swapChain->Present(0, 0);
	// context->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());
}
