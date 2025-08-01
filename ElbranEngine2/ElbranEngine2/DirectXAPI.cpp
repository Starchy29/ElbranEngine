#ifdef WINDOWS
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "DirectXAPI.h"
#include "Application.h"
#include <cassert>
#include <WICTextureLoader.h>

#define SafeRelease(x) if(x) x->Release()

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

	// create the viewport
	backBuffer = {};
	Resize(windowDims, viewAspectRatio);

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

	// set the input layout to match the Vertex struct in GraphicsAPI.h
	D3D11_INPUT_ELEMENT_DESC inputDescriptions[2];
	inputDescriptions[0] = { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT };
	inputDescriptions[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT };

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

void DirectXAPI::Release() {
	ReleaseRenderTarget(&backBuffer);
	ReleaseRenderTarget(&postProcessTargets[0]);
	ReleaseRenderTarget(&postProcessTargets[1]);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		ReleaseRenderTarget(&postProcessHelpers[i]);
	}
	GraphicsAPI::Release();
}

void DirectXAPI::Resize(Int2 windowDims, float viewAspectRatio) {
	this->viewAspectRatio = viewAspectRatio;
	depthStencilView.Reset();
	ReleaseRenderTarget(&backBuffer);
	swapChain->ResizeBuffers(2, windowDims.x, windowDims.y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// recreate back buffer
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer.data));
	device->CreateRenderTargetView(backBuffer.data, 0, &backBuffer.outputView);

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
	device->CreateDepthStencilView(depthBufferTexture, 0, depthStencilView.ReleaseAndGetAddressOf());
	depthBufferTexture->Release();

	// bind to the pipeline
	context->OMSetRenderTargets(1, &backBuffer.outputView, depthStencilView.Get());

	// set viewport
	float windowAspectRatio = (float)windowDims.x / windowDims.y;
	viewportOffset = UInt2(0, 0);
	viewportDims = UInt2(windowDims.x, windowDims.y);
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
	ReleaseRenderTarget(&postProcessTargets[0]);
	ReleaseRenderTarget(&postProcessTargets[1]);
	postProcessTargets[0] = CreateRenderTarget(windowDims.x, windowDims.y);
	postProcessTargets[1] = CreateRenderTarget(windowDims.x, windowDims.y);
	for(int i = 0; i < MAX_POST_PROCESS_HELPER_TEXTURES; i++) {
		ReleaseRenderTarget(&postProcessHelpers[i]);
		postProcessHelpers[i] = CreateRenderTarget(windowDims.x, windowDims.y);
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
	HRESULT result = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create vertex shader");
	newShader.constants = LoadConstantBuffer(shaderBlob);
	shaderBlob->Release();
	return newShader;
}

GeometryShader DirectXAPI::LoadGeometryShader(std::wstring directory, std::wstring fileName) {
	ID3DBlob* shaderBlob = LoadShader(directory, fileName);
	GeometryShader newShader = {};
	HRESULT result = device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create geometry shader");
	newShader.constants = LoadConstantBuffer(shaderBlob);
	shaderBlob->Release();
	return newShader;
}

PixelShader DirectXAPI::LoadPixelShader(std::wstring directory, std::wstring fileName) {
	ID3DBlob* shaderBlob = LoadShader(directory, fileName);
	PixelShader newShader = {};
	HRESULT result = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create pixel shader");
	newShader.constants = LoadConstantBuffer(shaderBlob);
	shaderBlob->Release();
	return newShader;
}

ComputeShader DirectXAPI::LoadComputeShader(std::wstring directory, std::wstring fileName) {
	ID3DBlob* shaderBlob = LoadShader(directory, fileName);
	ComputeShader newShader = {};
	HRESULT result = device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, &newShader.shader);
	assert(result == S_OK && "failed to create compute shader");
	newShader.constants = LoadConstantBuffer(shaderBlob);

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)reflection.GetAddressOf());
	reflection->GetThreadGroupSize(&newShader.xGroupSize, &newShader.yGroupSize, &newShader.zGroupSize);

	shaderBlob->Release();
	return newShader;
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

ConstantBuffer DirectXAPI::CreateConstantBuffer(unsigned int byteLength) {
	ConstantBuffer result = {};

	D3D11_BUFFER_DESC description;
	description.Usage = D3D11_USAGE_DYNAMIC;
	description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	description.MiscFlags = 0;
	description.StructureByteStride = 0;
	description.ByteWidth = (byteLength + 15) / 16 * 16; // must be a multiple of 16

	device->CreateBuffer(&description, 0, &result.data);
	return result;
}

ArrayBuffer DirectXAPI::CreateArrayBuffer(ShaderDataType type, unsigned int elements, unsigned int structBytes) {
	ArrayBuffer result = {};
	bool structured = type == ShaderDataType::Structured;
	result.buffer = CreateIndexedBuffer(elements, structured ? structBytes: ByteLengthOf(type), structured, true, false);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription;
	srvDescription.Format = FormatOf(type);
	srvDescription.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDescription.Buffer.FirstElement = 0;
	srvDescription.Buffer.NumElements = elements;

	device->CreateShaderResourceView(result.buffer, &srvDescription, &result.view);
	return result;
}

EditBuffer DirectXAPI::CreateEditBuffer(ShaderDataType type, unsigned int elements, unsigned int structBytes) {
	bool structured = type == ShaderDataType::Structured;
	unsigned int elementBytes = structured ? structBytes : ByteLengthOf(type);
	DXGI_FORMAT format = FormatOf(type);

	EditBuffer result = {};
	result.buffer = CreateIndexedBuffer(elements, elementBytes, structured, false, true);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription;
	srvDescription.Format = format;
	srvDescription.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDescription.Buffer.FirstElement = 0;
	srvDescription.Buffer.NumElements = elements;

	device->CreateShaderResourceView(result.buffer, &srvDescription, &result.view);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDescription;
	uavDescription.Format = format;
	uavDescription.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDescription.Buffer.FirstElement = 0;
	uavDescription.Buffer.NumElements = elements;
	uavDescription.Buffer.Flags = 0;

	device->CreateUnorderedAccessView(result.buffer, &uavDescription, &result.computeView);
	return result;
}

OutputBuffer DirectXAPI::CreateOutputBuffer(ShaderDataType type, unsigned int elements, unsigned int structBytes) {
	bool structured = type == ShaderDataType::Structured;
	unsigned int elementBytes = structured ? structBytes : ByteLengthOf(type);

	OutputBuffer result = {};
	result.gpuBuffer = CreateIndexedBuffer(elements, elementBytes, structured, false, true);
	result.cpuBuffer = CreateIndexedBuffer(elements, elementBytes, structured, true, true);
	result.byteLength = elements * elementBytes;

	D3D11_UNORDERED_ACCESS_VIEW_DESC description;
	description.Format = FormatOf(type);
	description.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	description.Buffer.FirstElement = 0;
	description.Buffer.NumElements = elements;
	description.Buffer.Flags = 0;

	device->CreateUnorderedAccessView(result.gpuBuffer, &description, &result.view);
	
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
	UnorderedAccessView* view = buffer ? buffer->computeView : nullptr;
	context->CSSetUnorderedAccessViews(slot, 1, &view, nullptr);
}

void DirectXAPI::WriteBuffer(const void* data, unsigned int byteLength, Buffer* buffer) {
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, byteLength);
	context->Unmap(buffer, 0);
}

void DirectXAPI::SetOutputBuffer(OutputBuffer* buffer, unsigned int slot, const void* initialData) {
	if(initialData) {
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		context->Map(buffer->cpuBuffer, 0, D3D11_MAP_WRITE, 0, &mappedResource);
		memcpy(mappedResource.pData, initialData, buffer->byteLength);
		context->Unmap(buffer->cpuBuffer, 0);
		context->CopyResource(buffer->gpuBuffer, buffer->cpuBuffer);
	}

	UnorderedAccessView* view = buffer ? buffer->view : nullptr;
	context->CSSetUnorderedAccessViews(slot, 1, &view, nullptr);
}

void DirectXAPI::ReadBuffer(const OutputBuffer* buffer, void* destination) {
	context->CopyResource(buffer->cpuBuffer, buffer->gpuBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(buffer->cpuBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
	memcpy(destination, mappedResource.pData, buffer->byteLength);
	context->Unmap(buffer->cpuBuffer, 0);
}

void DirectXAPI::SetPrimitive(RenderPrimitive primitive) {
	switch(primitive) {
	case RenderPrimitive::Point:
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case RenderPrimitive::Line:
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case RenderPrimitive::Triangle:
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	}
}

void DirectXAPI::SetBlendMode(BlendState mode) {
	switch(mode) {
	case BlendState::None:
		context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
		break;
	case BlendState::AlphaBlend:
		context->OMSetBlendState(alphaBlendState.Get(), nullptr, 0xffffffff);
		break;
	case BlendState::Additive:
		context->OMSetBlendState(additiveBlendState.Get(), nullptr, 0xffffffff);
		break;
	}
}

void DirectXAPI::SetConstants(ShaderStage stage, const ConstantBuffer* constantBuffer, unsigned int slot) {
	Buffer* data = constantBuffer ? constantBuffer->data : nullptr;
	switch (stage) {
	case ShaderStage::Vertex:
		context->VSSetConstantBuffers(slot, 1, &data);
		break;
	case ShaderStage::Geometry:
		context->GSSetConstantBuffers(slot, 1, &data);
		break;
	case ShaderStage::Pixel:
		context->PSSetConstantBuffers(slot, 1, &data);
		break;
	case ShaderStage::Compute:
		context->CSSetConstantBuffers(slot, 1, &data);
		break;
	}
}

void DirectXAPI::SetArray(ShaderStage stage, const ArrayBuffer* buffer, unsigned int slot) {
	ShaderResourceView* view = buffer ? buffer->view : nullptr;
	switch(stage) {
	case ShaderStage::Vertex:
		context->VSSetShaderResources(slot, 1, &view);
		break;
	case ShaderStage::Geometry:
		context->GSSetShaderResources(slot, 1, &view);
		break;
	case ShaderStage::Pixel:
		context->PSSetShaderResources(slot, 1, &view);
		break;
	case ShaderStage::Compute:
		context->CSSetShaderResources(slot, 1, &view);
		break;
	}
}

void DirectXAPI::SetTexture(ShaderStage stage, const Texture2D* texture, unsigned int slot) {
	ShaderResourceView* view = texture ? texture->inputView : nullptr;
	switch(stage) {
	case ShaderStage::Vertex:
		context->VSSetShaderResources(slot, 1, &view);
		break;
	case ShaderStage::Geometry:
		context->GSSetShaderResources(slot, 1, &view);
		break;
	case ShaderStage::Pixel:
		context->PSSetShaderResources(slot, 1, &view);
		break;
	case ShaderStage::Compute:
		context->CSSetShaderResources(slot, 1, &view);
		break;
	}
}

void DirectXAPI::SetSampler(ShaderStage stage, Sampler* sampler, unsigned int slot) {
	SamplerState* state = sampler ? sampler->state : nullptr;
	switch(stage) {
	case ShaderStage::Vertex:
		context->VSSetSamplers(slot, 1, &state);
		break;
	case ShaderStage::Geometry:
		context->GSSetSamplers(slot, 1, &state);
		break;
	case ShaderStage::Pixel:
		context->PSSetSamplers(slot, 1, &state);
		break;
	case ShaderStage::Compute:
		context->CSSetSamplers(slot, 1, &state);
		break;
	}
}

void DirectXAPI::SetComputeTexture(const ComputeTexture* texture, unsigned int slot) {
	UnorderedAccessView* view = texture ? texture->outputView : nullptr;
	context->CSSetUnorderedAccessViews(slot, 1, &view, nullptr);
}

void DirectXAPI::ClearMesh() {
	UINT zero = 0;
	ID3D11Buffer* nullBuffer = nullptr;
	context->IASetVertexBuffers(0, 1, &nullBuffer, &zero, &zero);
}

void DirectXAPI::SetVertexShader(const VertexShader* shader) {
	context->VSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::SetGeometryShader(const GeometryShader* shader) {
	context->GSSetShader(shader ? shader->shader : nullptr, 0, 0);
}

void DirectXAPI::SetPixelShader(const PixelShader* shader) {
	context->PSSetShader(shader->shader, 0, 0);
}

void DirectXAPI::RunComputeShader(const ComputeShader* shader, unsigned int xThreads, unsigned int yThreads, unsigned int zThreads) {
	context->CSSetShader(shader->shader, 0, 0);
	context->Dispatch(ceil((double)xThreads / shader->xGroupSize), ceil((double)yThreads / shader->yGroupSize), ceil((double)zThreads / shader->zGroupSize));
}

void DirectXAPI::SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil) {
	context->OMSetRenderTargets(1, &(renderTarget->outputView), useDepthStencil ? depthStencilView.Get() : nullptr);
}

void DirectXAPI::ReleaseShader(VertexShader* shader) {
	SafeRelease(shader->shader);
	SafeRelease(shader->constants.data);
}

void DirectXAPI::ReleaseShader(GeometryShader* shader) {
	SafeRelease(shader->shader);
	SafeRelease(shader->constants.data);
}

void DirectXAPI::ReleaseShader(PixelShader* shader) {
	SafeRelease(shader->shader);
	SafeRelease(shader->constants.data);
}

void DirectXAPI::ReleaseShader(ComputeShader* shader) {
	SafeRelease(shader->shader);
	SafeRelease(shader->constants.data);
}

void DirectXAPI::ReleaseSampler(Sampler* sampler) {
	SafeRelease(sampler->state);
}

void DirectXAPI::ReleaseTexture(Texture2D* texture) {
	SafeRelease(texture->data);
	SafeRelease(texture->inputView);
}

void DirectXAPI::ReleaseRenderTarget(RenderTarget* texture) {
	ReleaseTexture(texture);
	SafeRelease(texture->outputView);
}

void DirectXAPI::ReleaseComputeTexture(ComputeTexture* texture) {
	ReleaseTexture(texture);
	SafeRelease(texture->outputView);
}

void DirectXAPI::ReleaseMesh(Mesh* mesh) {
	SafeRelease(mesh->vertices);
	SafeRelease(mesh->indices);
}

void DirectXAPI::ReleaseConstantBuffer(ConstantBuffer* buffer) {
	SafeRelease(buffer->data);
}

void DirectXAPI::ReleaseArrayBuffer(ArrayBuffer* buffer) {
	SafeRelease(buffer->buffer);
	SafeRelease(buffer->view);
}

void DirectXAPI::ReleaseEditBuffer(EditBuffer* buffer) {
	ReleaseArrayBuffer(buffer);
	SafeRelease(buffer->computeView);
}

void DirectXAPI::ReleaseOuputBuffer(OutputBuffer* buffer) {
	SafeRelease(buffer->cpuBuffer);
	SafeRelease(buffer->gpuBuffer);
	SafeRelease(buffer->view);
}

void DirectXAPI::ClearBackBuffer() {
	float black[4] { 0.f, 0.f, 0.f, 0.f };
	context->ClearRenderTargetView(backBuffer.outputView, black);
}

void DirectXAPI::ClearDepthStencil() {
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DirectXAPI::PresentSwapChain() {
	swapChain->Present(0, 0);
}

void DirectXAPI::ResetRenderTarget() {
	context->OMSetRenderTargets(1, &backBuffer.outputView, depthStencilView.Get());
}

RenderTarget* DirectXAPI::GetBackBuffer() {
	return &backBuffer;
}

Texture2D DirectXAPI::LoadSprite(std::wstring directory, std::wstring fileName) {
	Texture2D result;
	std::wstring fullPath = directory + L"Assets\\" + fileName;
	ID3D11Resource* textureResource;
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), fullPath.c_str(), &textureResource, &result.inputView);
	result.data = (TextureData*)textureResource;

	D3D11_TEXTURE2D_DESC description;
	result.data->GetDesc(&description);
	result.aspectRatio = (float)description.Width / description.Height;
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

TextureData* DirectXAPI::CreateTexture(unsigned int width, int unsigned height, bool renderTarget, bool computeWritable) {
	TextureData* output;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = (renderTarget || computeWritable) ? 1 : 0; // include all mip levels unless gpu writable
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

Buffer* DirectXAPI::CreateIndexedBuffer(unsigned int elements, unsigned int elementBytes, bool structured, bool cpuWrite, bool gpuWrite) {
	Buffer* result;

	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.ByteWidth = elements * elementBytes;
	bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if(gpuWrite && cpuWrite) {
		bufferDescription.BindFlags = 0;
	}
	else if(gpuWrite) {
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

ConstantBuffer DirectXAPI::LoadConstantBuffer(ID3DBlob* shaderBlob) {
	// determine the byte length of the buffer
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)reflection.GetAddressOf());
	D3D11_SHADER_DESC shaderDescr;
	reflection->GetDesc(&shaderDescr);
	
	for(int i = 0; i < shaderDescr.ConstantBuffers; i++) {
		// find the constant buffer at register 0
		ID3D11ShaderReflectionConstantBuffer* bufferReflection = reflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDescr;
		bufferReflection->GetDesc(&bufferDescr);

		D3D11_SHADER_INPUT_BIND_DESC bindDescr;
		reflection->GetResourceBindingDescByName(bufferDescr.Name, &bindDescr);
		if(bindDescr.Type == D3D_SIT_CBUFFER && bindDescr.BindPoint == OBJECT_CONSTANT_REGISTER) {
			return CreateConstantBuffer(bufferDescr.Size);
		}
	}

	return {};
}

DXGI_FORMAT DirectXAPI::FormatOf(ShaderDataType type) {
	switch(type) {
	case ShaderDataType::Structured: return DXGI_FORMAT_UNKNOWN;

	case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
	case ShaderDataType::Int: return DXGI_FORMAT_R32_SINT;
	case ShaderDataType::UInt: return DXGI_FORMAT_R32_UINT;

	case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
	case ShaderDataType::Int2: return DXGI_FORMAT_R32G32_SINT;
	case ShaderDataType::UInt2: return DXGI_FORMAT_R32G32_UINT;

	case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case ShaderDataType::Int3: return DXGI_FORMAT_R32G32B32_SINT;
	case ShaderDataType::UInt3: return DXGI_FORMAT_R32G32B32_UINT;

	case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ShaderDataType::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;
	case ShaderDataType::UInt4: return DXGI_FORMAT_R32G32B32A32_UINT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

unsigned int DirectXAPI::ByteLengthOf(ShaderDataType type) {
	switch(type) {
	case ShaderDataType::Float: return sizeof(float);
	case ShaderDataType::Int: return sizeof(int);
	case ShaderDataType::UInt: return sizeof(unsigned int);

	case ShaderDataType::Float2: return 2 * sizeof(float);
	case ShaderDataType::Int2: return 2 * sizeof(int);
	case ShaderDataType::UInt2: return 2 * sizeof(unsigned int);

	case ShaderDataType::Float3: return 3 * sizeof(float);
	case ShaderDataType::Int3: return 3 * sizeof(int);
	case ShaderDataType::UInt3: return 3 * sizeof(unsigned int);

	case ShaderDataType::Float4: return 4 * sizeof(float);
	case ShaderDataType::Int4: return 4 * sizeof(int);
	case ShaderDataType::UInt4: return 4 * sizeof(unsigned int);
	}
	return 0;
}
#endif