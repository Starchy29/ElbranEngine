#ifdef WINDOWS
#include "DirectXAPI.h"
#include <cassert>

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
	postProcessTargets[0].Release(this);
	postProcessTargets[1].Release(this);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		postProcessHelpers[i].Release(this);
	}
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
	postProcessTargets[0].Release(this);
	postProcessTargets[1].Release(this);
	postProcessTargets[0] = CreateRenderTarget(viewportDims.x, viewportDims.y);
	postProcessTargets[1] = CreateRenderTarget(viewportDims.x, viewportDims.y);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		postProcessHelpers[i].Release(this);
		postProcessHelpers[i] = CreateRenderTarget(viewportDims.x, viewportDims.y);
	}
}

void DirectXAPI::DrawVertices(unsigned int numVertices) {
	context->Draw(numVertices, 0);
}

void DirectXAPI::DrawMesh(const Mesh* mesh) {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &(mesh->vertices), &stride, &offset);
	context->IASetIndexBuffer(mesh->indices, DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(mesh->indexCount, 0, 0);
}

Texture2D* DirectXAPI::CreateTexture(unsigned int width, int unsigned height, bool renderTarget, bool computeWritable) {
	Texture2D* output;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 0; // include all mip levels
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.Usage = (renderTarget || computeWritable ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE);
	textureDesc.CPUAccessFlags = 0;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if(renderTarget) {
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	else if(computeWritable) {
		textureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}
	
	device->CreateTexture2D(&textureDesc, 0, &output);

	return output;
}

Mesh DirectXAPI::CreateMesh(const Vertex* vertices, int vertexCount, const int* indices, int indexCount, bool editable) {
	Mesh result = {};
	result.indexCount = indexCount;

	// create the vertex buffer
	D3D11_BUFFER_DESC vertexDescription = {};
	vertexDescription.Usage = editable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	vertexDescription.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDescription.CPUAccessFlags = editable ? D3D11_CPU_ACCESS_WRITE : 0;
	vertexDescription.MiscFlags = 0;
	vertexDescription.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = vertices;

	device->CreateBuffer(&vertexDescription, &initialData, &(result.vertices));

	// create the index buffer
	D3D11_BUFFER_DESC indexDescription = {};
	indexDescription.Usage = D3D11_USAGE_IMMUTABLE;
	indexDescription.ByteWidth = sizeof(unsigned int) * indexCount;
	indexDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDescription.CPUAccessFlags = 0;
	indexDescription.MiscFlags = 0;
	indexDescription.StructureByteStride = 0;

	initialData.pSysMem = indices;

	device->CreateBuffer(&indexDescription, &initialData, &(result.indices));

	return result;
}

RenderTarget DirectXAPI::CreateRenderTarget(unsigned int width, unsigned int height) {
	RenderTarget result = {};
	result.texture = CreateTexture(width, height, true, false);

	device->CreateShaderResourceView(result.texture, 0, &(result.inputView));
	device->CreateRenderTargetView(result.texture, 0, &(result.outputView));

	return result;
}

ComputeTexture DirectXAPI::CreateComputeTexture(unsigned int width, unsigned int height) {
	ComputeTexture result = {};
	result.texture = CreateTexture(width, height, false, true);

	device->CreateShaderResourceView(result.texture, 0, &(result.inputView));
	device->CreateUnorderedAccessView(result.texture, 0, &(result.outputView));

	return result;
}

void DirectXAPI::CopyTexture(Texture2D* source, Texture2D* destination) {
	context->CopyResource(destination, source);
}

void DirectXAPI::ReleaseData(void* gpuData) {
	if(gpuData) {
		((IUnknown*)gpuData)->Release();
	}
}

void DirectXAPI::SetEditBuffer(EditBuffer* buffer, unsigned int slot) {
	context->CSSetUnorderedAccessViews(slot, 0, &(buffer->view), nullptr);
}

void DirectXAPI::WriteBuffer(const void* data, int byteLength, Buffer* buffer) {
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, byteLength);
	context->Unmap(buffer, 0);
}

void DirectXAPI::SetOutputBuffer(OutputBuffer* buffer, unsigned int slot, const void* initialData) {
	if(initialData) {
		WriteBuffer(initialData, buffer->byteLength, buffer->cpuBuffer);
		context->CopyResource(buffer->gpuBuffer, buffer->cpuBuffer);
	}

	context->CSSetUnorderedAccessViews(slot, 0, &(buffer->view), nullptr);
}

void DirectXAPI::ReadBuffer(const OutputBuffer* buffer, void* destination) {
	context->CopyResource(buffer->cpuBuffer, buffer->gpuBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(buffer->cpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(destination, mappedResource.pData, buffer->byteLength);
	context->Unmap(buffer->cpuBuffer, 0);
}

void DirectXAPI::LoadArray(Shader* shader, const EditBuffer* source, unsigned int slot) {
	context->CopyResource(shader->arrayBuffers[slot].buffer, source->buffer);
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

void DirectXAPI::SetComputeTexture(const ComputeTexture* texture, unsigned int slot) {
	context->CSSetUnorderedAccessViews(slot, 1, &(texture->outputView), nullptr);
}

void DirectXAPI::SetVertexShader(const VertexShader* shader) {
	for(unsigned int i = 0; i < shader->numConstBuffers; i++) {
		context->VSSetConstantBuffers(shader->constantBuffers[i].inputSlot, 1, &shader->constantBuffers[i].buffer);
	}

	for(unsigned int i = 0; i < shader->numArrayBuffers; i++) {
		context->VSSetShaderResources(shader->arrayBuffers[i].inputSlot, 1, &shader->arrayBuffers[i].view);
	}

	context->VSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::SetGeometryShader(const GeometryShader* shader) {
	for(unsigned int i = 0; i < shader->numConstBuffers; i++) {
		context->GSSetConstantBuffers(shader->constantBuffers[i].inputSlot, 1, &shader->constantBuffers[i].buffer);
	}

	for(unsigned int i = 0; i < shader->numArrayBuffers; i++) {
		context->GSSetShaderResources(shader->arrayBuffers[i].inputSlot, 1, &shader->arrayBuffers[i].view);
	}

	context->GSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::SetPixelShader(const PixelShader* shader) {
	for(unsigned int i = 0; i < shader->numConstBuffers; i++) {
		context->PSSetConstantBuffers(shader->constantBuffers[i].inputSlot, 1, &shader->constantBuffers[i].buffer);
	}

	for(unsigned int i = 0; i < shader->numArrayBuffers; i++) {
		context->PSSetShaderResources(shader->arrayBuffers[i].inputSlot, 1, &shader->arrayBuffers[i].view);
	}

	context->PSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::SetComputeShader(const ComputeShader* shader) {
	for(unsigned int i = 0; i < shader->numConstBuffers; i++) {
		context->CSSetConstantBuffers(shader->constantBuffers[i].inputSlot, 1, &shader->constantBuffers[i].buffer);
	}

	for(unsigned int i = 0; i < shader->numArrayBuffers; i++) {
		context->CSSetShaderResources(shader->arrayBuffers[i].inputSlot, 1, &shader->arrayBuffers[i].view);
	}

	context->CSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::SetRenderTarget(const RenderTarget* renderTarget) {
	context->OMSetRenderTargets(1, &(renderTarget->outputView), nullptr);
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
}

void DirectXAPI::ResetRenderTarget() {
	context->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());
}

RenderTarget DirectXAPI::GetBackBufferView() {
	RenderTarget result = {};
	result.outputView = backBufferView.Get();
	return result;
}
#endif