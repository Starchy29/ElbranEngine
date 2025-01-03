#include "Shader.h"
#include "Application.h"

Shader::Shader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) {
	this->dxDevice = device;
	this->dxContext = context;
}

Shader::~Shader() {
	for(UINT i = 0; i < cBuffCount; i++) {
		delete[] constantBuffers[i].localData;
	}
	delete[] constantBuffers;
}

// saves data for a variable into the correct spot of that buffer's localData. The resource is updated all at once in SetShader()
void Shader::SetConstantVariable(std::string name, const void* data) {
	std::unordered_map<std::string, ConstantVariable>::iterator element = constantVars.find(name);
	assert(element != constantVars.end() && "shader constant variable not found");

	ConstantVariable variable = element->second;
	ConstantBuffer buffer = constantBuffers[variable.bufferIndex];
	constantBuffers[variable.bufferIndex].needsUpdate = true;

	memcpy(buffer.localData + variable.byteOffset, data, variable.size);
}

void Shader::SetShader() {
	for(UINT i = 0; i < cBuffCount; i++) {
		ConstantBuffer* buffer = &(constantBuffers[i]);

		if(buffer->needsUpdate) {
			buffer->needsUpdate = false;

			D3D11_MAPPED_SUBRESOURCE mappedResource = {};
			dxContext->Map(buffer->cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(mappedResource.pData, buffer->localData, buffer->size);
			dxContext->Unmap(buffer->cBuffer.Get(), 0);
		}

		SetConstantBuffer(buffer->bindIndex, buffer->cBuffer);
	}

	SetSpecificShader();
}

void Shader::SetBool(std::string name, bool value) {
	// HLSL bools are 4 bytes. C++ bools are 1 byte. This function accounts for that discrepency
	__int32 hlslBool = value ? 1 : 0;
	SetConstantVariable(name, &hlslBool);
}

void Shader::LoadShader(std::wstring fileName) {
	// get the blob data
	std::wstring fileString = APP->ExePath() + fileName;
	LPCWSTR filePath = fileString.c_str();
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	HRESULT hr = D3DReadFileToBlob(filePath, shaderBlob.GetAddressOf());
	assert(hr == S_OK && "failed to read shader file");

	// get the info of this shader
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)reflection.GetAddressOf()
	);

	D3D11_SHADER_DESC shaderDescr;
	reflection->GetDesc(&shaderDescr);

	// set up the constant buffer information
	cBuffCount = shaderDescr.ConstantBuffers;
	for(UINT buffIndex = 0; buffIndex < shaderDescr.ConstantBuffers; buffIndex++) {
		// filter out the global light buffers
		ID3D11ShaderReflectionConstantBuffer* bufferReflection = reflection->GetConstantBufferByIndex(buffIndex);
		D3D11_SHADER_BUFFER_DESC bufferDescr;
		bufferReflection->GetDesc(&bufferDescr);

		D3D11_SHADER_INPUT_BIND_DESC bindDescr;
		reflection->GetResourceBindingDescByName(bufferDescr.Name, &bindDescr);

		if(bindDescr.BindPoint == LIGHT_CONSTANTS_REGISTER || bindDescr.BindPoint == LIGHT_ARRAY_REGISTER) {
			cBuffCount--;
		}
	}

	int nextIndex = 0;
	constantBuffers = new ConstantBuffer[cBuffCount];
	for(UINT i = 0; i < shaderDescr.ConstantBuffers; i++) { // assumes light buffers are after all other buffers
		ID3D11ShaderReflectionConstantBuffer* bufferReflection = reflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDescr;
		bufferReflection->GetDesc(&bufferDescr);

		D3D11_SHADER_INPUT_BIND_DESC bindDescr;
		reflection->GetResourceBindingDescByName(bufferDescr.Name, &bindDescr);

		if(bindDescr.BindPoint == LIGHT_CONSTANTS_REGISTER || bindDescr.BindPoint == LIGHT_ARRAY_REGISTER) {
			continue;
		}

		// create the constant buffer
		D3D11_BUFFER_DESC creationDescr;
		creationDescr.ByteWidth = ((bufferDescr.Size + 15) / 16) * 16; // force byte alignment
		creationDescr.Usage = D3D11_USAGE_DYNAMIC;
		creationDescr.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		creationDescr.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		creationDescr.MiscFlags = 0;
		creationDescr.StructureByteStride = 0;

		ConstantBuffer newBuffer = {};
		newBuffer.needsUpdate = false;
		newBuffer.size = bufferDescr.Size;
		newBuffer.bindIndex = bindDescr.BindPoint;
		dxDevice->CreateBuffer(&creationDescr, 0, newBuffer.cBuffer.GetAddressOf());
		newBuffer.localData = new BYTE[bufferDescr.Size];
		ZeroMemory(newBuffer.localData, bufferDescr.Size);

		constantBuffers[nextIndex] = newBuffer;

		// set up each variable within the constant buffer
		for(UINT varIndex = 0; varIndex < bufferDescr.Variables; varIndex++) {
			ID3D11ShaderReflectionVariable* variableReflection = bufferReflection->GetVariableByIndex(varIndex);
			D3D11_SHADER_VARIABLE_DESC varDescr;
			variableReflection->GetDesc(&varDescr);

			ConstantVariable variable = {};
			variable.bufferIndex = nextIndex;
			variable.byteOffset = varDescr.StartOffset;
			variable.size = varDescr.Size;

			constantVars.insert(std::pair<std::string, ConstantVariable>(varDescr.Name, variable));
		}

		nextIndex++;
	}

	CreateShader(shaderBlob);
}

ArrayBuffer Shader::CreateArrayBuffer(UINT elements, UINT elementBytes, ShaderDataType dataType) {
	Microsoft::WRL::ComPtr<ID3D11Device> dxDevice = APP->Graphics()->GetDevice();
	ArrayBuffer result = {};
	result.elements = elements;
	result.elementSize = elementBytes;

	// create the buffer
	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.ByteWidth = elements * elementBytes;
	bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if(dataType == ShaderDataType::Structured) {
		bufferDescription.StructureByteStride = elementBytes;
		bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	dxDevice->CreateBuffer(&bufferDescription, 0, result.buffer.GetAddressOf());

	// create the view
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription = {};
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	viewDescription.Format = FormatOfType(dataType);

	viewDescription.Buffer.FirstElement = 0;
	viewDescription.Buffer.ElementOffset = 0;
	viewDescription.Buffer.ElementWidth = elementBytes;
	viewDescription.Buffer.NumElements = elements;

	dxDevice->CreateShaderResourceView(result.buffer.Get(), &viewDescription, result.view.GetAddressOf());

	return result;
}

void Shader::WriteArray(const void* data, ArrayBuffer* destination) {
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = APP->Graphics()->GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	context->Map(destination->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, destination->elements * destination->elementSize);
	context->Unmap(destination->buffer.Get(), 0);
}

DXGI_FORMAT Shader::FormatOfType(ShaderDataType type) {
	switch(type) {
	case ShaderDataType::Structured:
		return DXGI_FORMAT_UNKNOWN;

	case ShaderDataType::Float:
		return DXGI_FORMAT_R32_FLOAT;
	case ShaderDataType::Int:
		return DXGI_FORMAT_R32_SINT;
	case ShaderDataType::UInt:
		return DXGI_FORMAT_R32_UINT;

	case ShaderDataType::Float2:
		return DXGI_FORMAT_R32G32_FLOAT;
	case ShaderDataType::Int2:
		return DXGI_FORMAT_R32G32_SINT;
	case ShaderDataType::UInt2:
		return DXGI_FORMAT_R32G32_UINT;

	case ShaderDataType::Float3:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case ShaderDataType::Int3:
		return DXGI_FORMAT_R32G32B32_SINT;
	case ShaderDataType::UInt3:
		return DXGI_FORMAT_R32G32B32_UINT;

	case ShaderDataType::Float4:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ShaderDataType::Int4:
		return DXGI_FORMAT_R32G32B32A32_SINT;
	case ShaderDataType::UInt4:
		return DXGI_FORMAT_R32G32B32A32_UINT;
	}

	return DXGI_FORMAT_UNKNOWN;
}
