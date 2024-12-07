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

	if(element == constantVars.end()) {
		throw std::invalid_argument("variable not found");
	}

	ConstantVariable variable = element->second;
	ConstantBuffer buffer = constantBuffers[variable.bufferIndex];
	constantBuffers[variable.bufferIndex].needsUpdate = true;

	memcpy(buffer.localData + variable.byteOffset, data, variable.size);
}

void Shader::SetShader() {
	for(UINT i = 0; i < cBuffCount; i++) {
		ConstantBuffer buffer = constantBuffers[i];

		if(buffer.needsUpdate) {
			buffer.needsUpdate = false;

			D3D11_MAPPED_SUBRESOURCE mappedResource = {};
			dxContext->Map(buffer.cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(mappedResource.pData, buffer.localData, buffer.size);
			dxContext->Unmap(buffer.cBuffer.Get(), 0);
		}

		SetConstantBuffer(i, buffer.cBuffer);
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
	if(hr != S_OK) {
		throw std::invalid_argument("failed to read shader file");
	}

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
	constantBuffers = new ConstantBuffer[cBuffCount];
	for(UINT buffIndex = 0; buffIndex < cBuffCount; buffIndex++) {
		ID3D11ShaderReflectionConstantBuffer* bufferReflection = reflection->GetConstantBufferByIndex(buffIndex);
		D3D11_SHADER_BUFFER_DESC bufferDescr;
		bufferReflection->GetDesc(&bufferDescr);

		D3D11_SHADER_INPUT_BIND_DESC bindDescr;
		reflection->GetResourceBindingDescByName(bufferDescr.Name, &bindDescr);

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
		dxDevice->CreateBuffer(&creationDescr, 0, newBuffer.cBuffer.GetAddressOf());
		newBuffer.localData = new BYTE[bufferDescr.Size];
		ZeroMemory(newBuffer.localData, bufferDescr.Size);

		constantBuffers[buffIndex] = newBuffer;

		// set up each variable within the constant buffer
		for(UINT varIndex = 0; varIndex < bufferDescr.Variables; varIndex++) {
			ID3D11ShaderReflectionVariable* variableReflection = bufferReflection->GetVariableByIndex(varIndex);
			D3D11_SHADER_VARIABLE_DESC varDescr;
			variableReflection->GetDesc(&varDescr);

			ConstantVariable variable = {};
			variable.bufferIndex = buffIndex;
			variable.byteOffset = varDescr.StartOffset;
			variable.size = varDescr.Size;

			constantVars.insert(std::pair<std::string, ConstantVariable>(varDescr.Name, variable));
		}
	}

	CreateShader(shaderBlob);
}
