#ifdef WINDOWS
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "DirectXAPI.h"
#include "Application.h"
#include <cassert>
#include <WICTextureLoader.h>

DirectXAPI::DirectXAPI(HWND windowHandle, Int2 windowDims, float viewAspectRatio, std::wstring directory) {
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

	device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);

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

	// set the input layout to match the Vertex struct in GraphicsAPI.h
	D3D11_INPUT_ELEMENT_DESC inputDescriptions[2];
	inputDescriptions[0] = { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT };
	inputDescriptions[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT };

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	std::wstring fileString = directory + L"CameraVS.cso";
	HRESULT hr = D3DReadFileToBlob(fileString.c_str(), shaderBlob.GetAddressOf()); // read a specific shader to validate input layout
	assert(hr == S_OK && "failed to read shader for input layout creation");

	ID3D11InputLayout* defaultLayout;
	device->CreateInputLayout(inputDescriptions, 2, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &defaultLayout);
	context->IASetInputLayout(defaultLayout);
	defaultLayout->Release();

	// set other defaults
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//spriteBatch = new DirectX::SpriteBatch(context.Get());
}

DirectXAPI::~DirectXAPI() {
	
}

void DirectXAPI::Resize(Int2 windowDims, float viewAspectRatio) {
	this->viewAspectRatio = viewAspectRatio;
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
	postProcessTargets[0].Release();
	postProcessTargets[1].Release();
	postProcessTargets[0] = CreateRenderTarget(viewportDims.x, viewportDims.y);
	postProcessTargets[1] = CreateRenderTarget(viewportDims.x, viewportDims.y);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		postProcessHelpers[i].Release();
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

VertexShader DirectXAPI::LoadVertexShader(std::wstring directory, std::wstring fileName) {
	ID3DBlob* shaderBlob = LoadShader(directory, fileName);
	VertexShader newShader = {};
	CreateBuffers(shaderBlob, &newShader);
	HRESULT result = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create vertex shader");
	shaderBlob->Release();
	return newShader;
}

GeometryShader DirectXAPI::LoadGeometryShader(std::wstring directory, std::wstring fileName) {
	ID3DBlob* shaderBlob = LoadShader(directory, fileName);
	GeometryShader newShader = {};
	CreateBuffers(shaderBlob, &newShader);
	HRESULT result = device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create geometry shader");
	shaderBlob->Release();
	return newShader;
}

PixelShader DirectXAPI::LoadPixelShader(std::wstring directory, std::wstring fileName) {
	ID3DBlob* shaderBlob = LoadShader(directory, fileName);
	PixelShader newShader = {};
	CreateBuffers(shaderBlob, &newShader);
	HRESULT result = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create pixel shader");
	shaderBlob->Release();
	return newShader;
}

ComputeShader DirectXAPI::LoadComputeShader(std::wstring directory, std::wstring fileName) {
	ID3DBlob* shaderBlob = LoadShader(directory, fileName);
	ComputeShader newShader = {};
	CreateBuffers(shaderBlob, &newShader);
	HRESULT result = device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create compute shader");

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)reflection.GetAddressOf());
	reflection->GetThreadGroupSize(&newShader.xGroupSize, &newShader.yGroupSize, &newShader.zGroupSize);

	shaderBlob->Release();
	return newShader;
}

TextureData* DirectXAPI::CreateTexture(unsigned int width, int unsigned height, bool renderTarget, bool computeWritable) {
	TextureData* output;

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

Mesh DirectXAPI::CreateMesh(const Vertex* vertices, int vertexCount, const unsigned int* indices, int indexCount, bool editable) {
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

ArrayBuffer DirectXAPI::CreateArrayBuffer(unsigned int elements, unsigned int elementBytes, bool structured) {
	ArrayBuffer result = {};
	result.buffer = CreateIndexedBuffer(elements, elementBytes, structured, true, false);
	device->CreateShaderResourceView(result.buffer, 0, &result.view);
	return result;
}

EditBuffer DirectXAPI::CreateEditBuffer(unsigned int elements, unsigned int elementBytes, bool structured) {
	EditBuffer result = {};
	result.buffer = CreateIndexedBuffer(elements, elementBytes, structured, false, true);
	device->CreateShaderResourceView(result.buffer, 0, &result.view);
	device->CreateUnorderedAccessView(result.buffer, 0, &result.computeView);
	return result;
}

OutputBuffer DirectXAPI::CreateOutputBuffer(unsigned int elements, unsigned int elementBytes, bool structured) {
	OutputBuffer result = {};
	result.gpuBuffer = CreateIndexedBuffer(elements, elementBytes, structured, false, false);
	device->CreateUnorderedAccessView(result.gpuBuffer, 0, &result.view);
	result.cpuBuffer = CreateIndexedBuffer(elements, elementBytes, structured, true, true);
	return result;
}

RenderTarget DirectXAPI::CreateRenderTarget(unsigned int width, unsigned int height) {
	RenderTarget result = {};
	result.aspectRatio = (float)width / height;
	result.data = CreateTexture(width, height, true, false);

	device->CreateShaderResourceView(result.data, 0, &(result.inputView));
	device->CreateRenderTargetView(result.data, 0, &(result.outputView));

	return result;
}

ComputeTexture DirectXAPI::CreateComputeTexture(unsigned int width, unsigned int height) {
	ComputeTexture result = {};
	result.aspectRatio = (float)width / height;
	result.data = CreateTexture(width, height, false, true);

	device->CreateShaderResourceView(result.data, 0, &(result.inputView));
	device->CreateUnorderedAccessView(result.data, 0, &(result.outputView));

	return result;
}

void DirectXAPI::CopyTexture(Texture2D* source, Texture2D* destination) {
	context->CopyResource(destination->data, source->data);
}

void DirectXAPI::SetEditBuffer(EditBuffer* buffer, unsigned int slot) {
	context->CSSetUnorderedAccessViews(slot, 0, &(buffer->computeView), nullptr);
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

void DirectXAPI::SetArray(ShaderStage stage, const ArrayBuffer* buffer, unsigned int slot) {
	switch(stage) {
	case ShaderStage::Vertex:
		context->VSSetShaderResources(slot, 1, &buffer->view);
		break;
	case ShaderStage::Geometry:
		context->GSSetShaderResources(slot, 1, &buffer->view);
		break;
	case ShaderStage::Pixel:
		context->PSSetShaderResources(slot, 1, &buffer->view);
		break;
	case ShaderStage::Compute:
		context->CSSetShaderResources(slot, 1, &buffer->view);
		break;
	}
}

void DirectXAPI::SetTexture(ShaderStage stage, const Texture2D* texture, unsigned int slot) {
	switch(stage) {
	case ShaderStage::Vertex:
		context->VSSetShaderResources(slot, 1, &texture->inputView);
		break;
	case ShaderStage::Geometry:
		context->GSSetShaderResources(slot, 1, &texture->inputView);
		break;
	case ShaderStage::Pixel:
		context->PSSetShaderResources(slot, 1, &texture->inputView);
		break;
	case ShaderStage::Compute:
		context->CSSetShaderResources(slot, 1, &texture->inputView);
		break;
	}
}

void DirectXAPI::SetSampler(ShaderStage stage, Sampler* sampler, unsigned int slot) {
	switch(stage) {
	case ShaderStage::Vertex:
		context->VSSetSamplers(slot, 1, &sampler->state);
		break;
	case ShaderStage::Geometry:
		context->GSSetSamplers(slot, 1, &sampler->state);
		break;
	case ShaderStage::Pixel:
		context->PSSetSamplers(slot, 1, &sampler->state);
		break;
	case ShaderStage::Compute:
		context->CSSetSamplers(slot, 1, &sampler->state);
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

	context->VSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::SetGeometryShader(const GeometryShader* shader) {
	for(unsigned int i = 0; i < shader->numConstBuffers; i++) {
		context->GSSetConstantBuffers(shader->constantBuffers[i].inputSlot, 1, &shader->constantBuffers[i].buffer);
	}

	context->GSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::SetPixelShader(const PixelShader* shader) {
	for(unsigned int i = 0; i < shader->numConstBuffers; i++) {
		context->PSSetConstantBuffers(shader->constantBuffers[i].inputSlot, 1, &shader->constantBuffers[i].buffer);
	}

	context->PSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::RunComputeShader(const ComputeShader* shader, unsigned int xThreads, unsigned int yThreads, unsigned int zThreads) {
	for(unsigned int i = 0; i < shader->numConstBuffers; i++) {
		context->CSSetConstantBuffers(shader->constantBuffers[i].inputSlot, 1, &shader->constantBuffers[i].buffer);
	}

	context->CSSetShader(shader->shader, 0, 0);
	context->Dispatch(ceil((double)xThreads / shader->xGroupSize), ceil((double)yThreads / shader->yGroupSize), ceil((double)zThreads / shader->zGroupSize));
}

void DirectXAPI::SetRenderTarget(const RenderTarget* renderTarget) {
	context->OMSetRenderTargets(1, &(renderTarget->outputView), nullptr);
}

void DirectXAPI::ClearBackBuffer() {
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

Texture2D DirectXAPI::LoadSprite(std::wstring directory, std::wstring fileName) {
	Texture2D result;
	std::wstring fullPath = directory + L"Assets\\" + fileName;
	ID3D11Resource* textureResource;
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), fullPath.c_str(), &textureResource, &result.inputView);
	result.data = (TextureData*)textureResource;
	return result;
}

Sampler DirectXAPI::CreateDefaultSampler() {
	Sampler result;

	D3D11_SAMPLER_DESC descriptiom = {};
	descriptiom.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descriptiom.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descriptiom.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descriptiom.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	descriptiom.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&descriptiom, &result.state);

	return result;
}

Buffer* DirectXAPI::CreateIndexedBuffer(unsigned int elements, unsigned int elementBytes, bool structured, bool cpuWrite, bool gpuWrite) {
	Buffer* result;

	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.ByteWidth = elements * elementBytes;
	bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if(gpuWrite) {
		bufferDescription.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	if(cpuWrite && gpuWrite) {
		bufferDescription.Usage = D3D11_USAGE_STAGING;
	}
	else if(cpuWrite) {
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	}

	bufferDescription.CPUAccessFlags = 0;
	if(cpuWrite) {
		bufferDescription.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}
	if(cpuWrite && gpuWrite) {
		bufferDescription.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
	}

	if(structured) {
		bufferDescription.StructureByteStride = elementBytes;
		bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	device->CreateBuffer(&bufferDescription, 0, &result);

	return result;
}

ID3DBlob* DirectXAPI::LoadShader(std::wstring directory, std::wstring fileName) {
	std::wstring fileString = directory + fileName;
	LPCWSTR filePath = fileString.c_str();
	ID3DBlob* shaderBlob;
	HRESULT hr = D3DReadFileToBlob(filePath, &shaderBlob);
	assert(hr == S_OK && "failed to read shader file");
	return shaderBlob;
}

void DirectXAPI::CreateBuffers(ID3DBlob* shaderBlob, Shader* output) {
	// get the info of this shader
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)reflection.GetAddressOf());

	D3D11_SHADER_DESC shaderDescr;
	reflection->GetDesc(&shaderDescr);

	// create the constant buffers
	ID3D11ShaderReflectionConstantBuffer* bufferReflection;
	D3D11_SHADER_BUFFER_DESC bufferDescr;
	D3D11_SHADER_INPUT_BIND_DESC bindDescr;
	output->numConstBuffers = shaderDescr.ConstantBuffers;
	for(UINT i = 0; i < shaderDescr.ConstantBuffers; i++) {
		// filter out certain buffers
		bufferReflection = reflection->GetConstantBufferByIndex(i);
		bufferReflection->GetDesc(&bufferDescr);
		reflection->GetResourceBindingDescByName(bufferDescr.Name, &bindDescr);
		if(bindDescr.Type != D3D_SIT_CBUFFER) {
			output->numConstBuffers--;
		}
	}

	D3D11_BUFFER_DESC creationDescr;
	creationDescr.Usage = D3D11_USAGE_DYNAMIC;
	creationDescr.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	creationDescr.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	creationDescr.MiscFlags = 0;
	creationDescr.StructureByteStride = 0;

	output->constantBuffers = new ConstantBuffer[output->numConstBuffers];
	int nextIndex = 0;
	for(UINT i = 0; i < shaderDescr.ConstantBuffers; i++) {
		bufferReflection = reflection->GetConstantBufferByIndex(i);
		bufferReflection->GetDesc(&bufferDescr);
		reflection->GetResourceBindingDescByName(bufferDescr.Name, &bindDescr);
		if(bindDescr.Type != D3D_SIT_CBUFFER) {
			continue;
		}

		// create the constant buffer
		ConstantBuffer* newBuffer = &(output->constantBuffers[nextIndex]);
		newBuffer->inputSlot = bindDescr.BindPoint;
		newBuffer->byteLength = bufferDescr.Size;
		creationDescr.ByteWidth = bufferDescr.Size;
		device->CreateBuffer(&creationDescr, 0, &newBuffer->buffer);
		nextIndex++;
	}
}
#endif