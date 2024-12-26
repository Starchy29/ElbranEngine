#include "ComputeShader.h"
#include "Application.h"

ComputeShader::ComputeShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName) 
	: Shader(device, context)
{
	LoadShader(fileName);
}

ComputeShader::~ComputeShader() { }

ReadWriteTexture ComputeShader::CreateReadWriteTexture(UINT width, UINT height) {
	Microsoft::WRL::ComPtr<ID3D11Device> device = APP->Graphics()->GetDevice();
	ReadWriteTexture result;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

	// create the texture
	D3D11_TEXTURE2D_DESC textureDescription = {};
	textureDescription.Width = width;
	textureDescription.Height = height;
	textureDescription.ArraySize = 1;
	textureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDescription.Usage = D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDescription.CPUAccessFlags = 0;

	device->CreateTexture2D(&textureDescription, 0, texture.GetAddressOf());

	// create the input view
	D3D11_SHADER_RESOURCE_VIEW_DESC inputDescription = {};
	inputDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	device->CreateShaderResourceView(texture.Get(), &inputDescription, result.srv.GetAddressOf());

	// create the output view
	D3D11_UNORDERED_ACCESS_VIEW_DESC outputDescription = {};
	outputDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	outputDescription.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	device->CreateUnorderedAccessView(texture.Get(), &outputDescription, result.uav.GetAddressOf());

	return result;
}

RWArrayBuffer ComputeShader::CreateReadWriteBuffer(UINT elements, UINT elementBytes, ShaderDataType dataType) {
	Microsoft::WRL::ComPtr<ID3D11Device> dxDevice = APP->Graphics()->GetDevice();
	RWArrayBuffer result = {};
	result.elements = elements;
	result.elementSize = elementBytes;

	// create the gpu read/write buffer
	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.ByteWidth = elements * elementBytes;
	if(dataType == ShaderDataType::Structured) {
		bufferDescription.StructureByteStride = elementBytes;
		bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufferDescription.CPUAccessFlags = 0;

	dxDevice->CreateBuffer(&bufferDescription, 0, result.gpuBuffer.GetAddressOf());

	// create the buffer that the cpu can read/write
	bufferDescription.Usage = D3D11_USAGE_STAGING;
	bufferDescription.BindFlags = 0;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	dxDevice->CreateBuffer(&bufferDescription, 0, result.cpuBuffer.GetAddressOf());

	// create the view
	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDescription = {};
	viewDescription.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	viewDescription.Format = FormatOfType(dataType);

	viewDescription.Buffer.FirstElement = 0;
	viewDescription.Buffer.Flags = 0;
	viewDescription.Buffer.NumElements = elements;
	
	dxDevice->CreateUnorderedAccessView(result.gpuBuffer.Get(), &viewDescription, result.view.GetAddressOf());

	return result;
}

void ComputeShader::WriteBuffer(void* data, RWArrayBuffer* buffer) {
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = APP->Graphics()->GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(buffer->cpuBuffer.Get(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
	memcpy(mappedResource.pData, data, buffer->elements * buffer->elementSize);
	context->Unmap(buffer->cpuBuffer.Get(), 0);

	context->CopyResource(buffer->gpuBuffer.Get(), buffer->cpuBuffer.Get());
}

void ComputeShader::ReadBuffer(RWArrayBuffer* buffer, void* destination) {
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = APP->Graphics()->GetContext();

	context->CopyResource(buffer->cpuBuffer.Get(), buffer->gpuBuffer.Get());

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(buffer->cpuBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
	memcpy(destination, mappedResource.pData, buffer->elements * buffer->elementSize);
	context->Unmap(buffer->cpuBuffer.Get(), 0);
}

void ComputeShader::SetReadWriteBuffer(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> view, int slot) {
	dxContext->CSSetUnorderedAccessViews(slot, 1, view.GetAddressOf(), nullptr);
}

void ComputeShader::SetSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, int slot) {
	dxContext->CSSetSamplers(slot, 1, samplerState.GetAddressOf());
}

void ComputeShader::SetArrayBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot) {
	dxContext->CSSetShaderResources(slot, 1, view.GetAddressOf());
}

void ComputeShader::Dispatch(int xSize, int ySize, int zSize) {
	SetShader();
	dxContext->Dispatch(ceil((double)xSize / xGroupSize), ceil((double)ySize / yGroupSize), ceil((double)zSize / zGroupSize));
}

void ComputeShader::SetSpecificShader() {
	dxContext->CSSetShader(dxShader.Get(), 0, 0);
}

void ComputeShader::CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) {
	HRESULT result = dxDevice->CreateComputeShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		dxShader.GetAddressOf()
	);

	if(result != S_OK) {
		throw std::exception("failed to create compute shader");
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)reflection.GetAddressOf()
	);

	// determine the thread group sizes
	reflection->GetThreadGroupSize(&xGroupSize, &yGroupSize, &zGroupSize);
}

void ComputeShader::SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer) {
	dxContext->CSSetConstantBuffers(slot, 1, cBuffer.GetAddressOf());
}
