#include "DXCore.h"
#include "Game.h"
#include "Application.h"

void DXCore::SetBlendMode(BlendState mode) {
	switch(mode) {
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

void DXCore::StartTextBatch() {
	spriteBatch->Begin(DirectX::DX11::SpriteSortMode_FrontToBack, 
		nullptr,
		nullptr,
		defaultStencil.Get()
	);
}

void DXCore::FinishTextBatch() {
	spriteBatch->End();

	context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	context->RSSetState(0);
	Mesh::ClearLastDrawn();
}

void DXCore::DrawScreen() {
	APP->Assets()->backgroundVS->SetShader();
	context->Draw(3, 0);
}

void DXCore::RunPostProcesses() {
	assert(!postProcessed && "attempted to run post processes twice");

	postProcessed = true;

	for(int i = 0; i < postProcesses.size(); i++) {
		PostProcessTexture* input;
		PostProcessTexture* output;
		if(i % 2 == 0) {
			input = &ppTex1;
			output = &ppTex2;
		} else {
			input = &ppTex2;
			output = &ppTex1;
		}

		if(postProcesses[i]->IsActive()) {
			postProcesses[i]->Render(input->GetShaderResource(), i == postProcesses.size() - 1 ? backBufferView : output->GetRenderTarget());
		} else {
			// if the post process makes no changes, copying the pixels is faster
			ID3D11Resource* outputResource;
			if(i == postProcesses.size() - 1) {
				backBufferView->GetResource(&outputResource);
			} else {
				outputResource = output->GetTexture().Get();
			}
			context->CopyResource(outputResource, input->GetTexture().Get());
			outputResource->Release();
		}
	}

	context->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());
}

void DXCore::SetLights(const Light* lights, int numLights, const Color& ambientColor) {
	Shader::WriteArray(lights, &lightBuffer);
	context->PSSetShaderResources(LIGHT_ARRAY_REGISTER, 1, lightBuffer.view.GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(lightConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, ambientColor, sizeof(Color));
	memcpy(((BYTE*)mappedResource.pData + sizeof(Color)), &numLights, sizeof(int));
	context->Unmap(lightConstantBuffer.Get(), 0);
	context->PSSetConstantBuffers(LIGHT_CONSTANTS_REGISTER, 1, lightConstantBuffer.GetAddressOf());
}

void DXCore::SetFullscreen(bool fullscreen) {
	swapChain->SetFullscreenState(fullscreen, nullptr);
}

bool DXCore::IsFullscreen() const {
	BOOL fullscreen;
	swapChain->GetFullscreenState(&fullscreen, nullptr);
	return fullscreen;
}

Microsoft::WRL::ComPtr<ID3D11Device> DXCore::GetDevice() const {
	return device;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> DXCore::GetContext() const {
	return context;
}

DirectX::XMINT2 DXCore::GetViewDimensions() const {
	return viewportDims;
}

DirectX::XMINT2 DXCore::GetViewOffset() const {
	return viewportShift;
}

DirectX::SpriteBatch* DXCore::GetSpriteBatch() const {
	return spriteBatch;
}

PostProcessTexture* DXCore::GetPostProcessTexture(int index) {
	assert(index >= 0 && index < MAX_POST_PROCESS_TEXTURES && "index was out of range");
	return &ppHelpers[index];
}

DXCore::DXCore(HWND windowHandle, DirectX::XMINT2 windowDims, float viewAspectRatio, HRESULT* outResult) {
	postProcessed = false;

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
	*outResult = D3D11CreateDevice(
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

	if(FAILED(*outResult)) return;

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

	*outResult = dxgiDevice->GetAdapter(&adapter);
	if(FAILED(*outResult)) return;

	adapter->GetParent(IID_PPV_ARGS(&factory));

	*outResult = factory->CreateSwapChain(
		device.Get(),
		&desc,
		&swapChain
	);

	if(FAILED(*outResult)) return;

	// create render target
	ID3D11Texture2D* backBuffer;
	*outResult = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&backBuffer
	);

	if(FAILED(*outResult)) return;

	*outResult = device->CreateRenderTargetView(
		backBuffer,
		nullptr,
		backBufferView.GetAddressOf()
	);

	if(FAILED(*outResult)) return;
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

	*outResult = device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depthStencil
	);

	if(FAILED(*outResult)) return;

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	*outResult = device->CreateDepthStencilView(
		depthStencil,
		&depthStencilViewDesc,
		&depthStencilView
	);

	if(FAILED(*outResult)) return;
	depthStencil->Release();

	D3D11_DEPTH_STENCIL_DESC stencilOMDesc = {};
	stencilOMDesc.DepthEnable = true;
	stencilOMDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stencilOMDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	stencilOMDesc.StencilEnable = false;
	stencilOMDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	stencilOMDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	stencilOMDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	stencilOMDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	stencilOMDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOMDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOMDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOMDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOMDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilOMDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&stencilOMDesc, defaultStencil.GetAddressOf());
	context->OMSetDepthStencilState(defaultStencil.Get(), 0);

	// create the blend state for alpha blending
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

	// create viewport
	Resize(windowDims, viewAspectRatio);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	spriteBatch = new DirectX::SpriteBatch(context.Get());

	*outResult = S_OK;
}

DXCore::~DXCore() {
	delete spriteBatch;
	for(IPostProcess* pp : postProcesses) {
		delete pp;
	}
}

void DXCore::SetupLighting() {
	lightBuffer = Shader::CreateArrayBuffer(MAX_LIGHTS_ONSCREEN, sizeof(Light), ShaderDataType::Structured);

	D3D11_BUFFER_DESC bufferDescription;
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags = 0;
	bufferDescription.StructureByteStride = 0;
	bufferDescription.ByteWidth = 32; // float4 + int, rounded up to nearest multiple of 16
	device->CreateBuffer(&bufferDescription, nullptr, lightConstantBuffer.GetAddressOf());
}

void DXCore::Resize(DirectX::XMINT2 windowDims, float viewAspectRatio) {
	backBufferView.Reset();
	depthStencilView.Reset();

	swapChain->ResizeBuffers(2, windowDims.x, windowDims.y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// recreate back buffer
	ID3D11Texture2D* backBufferTexture = 0;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
	if(backBufferTexture) {
		device->CreateRenderTargetView(
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
	device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	if(depthBufferTexture != 0) {
		device->CreateDepthStencilView(
			depthBufferTexture,
			0,
			depthStencilView.ReleaseAndGetAddressOf()
		);
		depthBufferTexture->Release();
	}

	// bind to the pipeline
	context->OMSetRenderTargets(
		1,
		backBufferView.GetAddressOf(),
		depthStencilView.Get()
	);

	// set viewport
	float windowAspectRatio = (float)windowDims.x / windowDims.y;
	viewportShift = DirectX::XMINT2(0, 0);
	viewportDims = windowDims;
	if(windowAspectRatio > viewAspectRatio) {
		viewportDims.x = windowDims.y * viewAspectRatio;
		viewportShift.x = (windowDims.x - viewportDims.x) / 2;
	} else {
		viewportDims.y = windowDims.x / viewAspectRatio;
		viewportShift.y = (windowDims.y - viewportDims.y) / 2;
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewportShift.x;
	viewport.TopLeftY = viewportShift.y;
	viewport.Width = (float)viewportDims.x;
	viewport.Height = (float)viewportDims.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	ppTex1.Resize(device, windowDims.x, windowDims.y);
	ppTex2.Resize(device, windowDims.x, windowDims.y);
	for(int i = 0; i < MAX_POST_PROCESS_TEXTURES; i++) {
		ppHelpers[i].Resize(device, windowDims.x, windowDims.y);
	}
}

void DXCore::Render(Game* game) {
	postProcessed = false;

	context->ClearRenderTargetView(backBufferView.Get(), Color::Black);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if(postProcesses.size() > 0) {
		context->OMSetRenderTargets(1, ppTex1.GetRenderTarget().GetAddressOf(), depthStencilView.Get());
	}

	game->Draw();

	if(!postProcessed && postProcesses.size() > 0) {
		RunPostProcesses();
	}

	swapChain->Present(0, 0);
	context->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());

	// Unbind shader resource views at the end of the frame
	ID3D11ShaderResourceView* nullSRVs[128] = {};
	context->PSSetShaderResources(0, ARRAYSIZE(nullSRVs), nullSRVs);
}
