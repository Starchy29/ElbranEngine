#include "VertexShader.h"
#include <vector>

VertexShader::VertexShader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::wstring fileName)
	: Shader(device, context)
{ 
	LoadShader(fileName);
}

void VertexShader::SetShader() {
	context->IASetInputLayout(inputLayout.Get());
	context->VSSetShader(dxShader.Get(), 0, 0);
}

void VertexShader::CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) {
	HRESULT result = device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		dxShader.GetAddressOf()
	);

	if(result != S_OK) {
		throw std::exception("failed to create vertex shader");
	}

	// get shader info
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)reflection.GetAddressOf()
	);

	D3D11_SHADER_DESC shaderDescription;
	reflection->GetDesc(&shaderDescription);

	// set input layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescription;
	for(UINT i = 0; i < shaderDescription.InputParameters; i++) {
		D3D11_SIGNATURE_PARAMETER_DESC parameterDescription;
		reflection->GetInputParameterDesc(i, &parameterDescription);

		// describe the input element
		D3D11_INPUT_ELEMENT_DESC elementDescription = {};
		elementDescription.SemanticName = parameterDescription.SemanticName;
		elementDescription.SemanticIndex = parameterDescription.SemanticIndex;
		elementDescription.InputSlot = 0;
		elementDescription.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDescription.InstanceDataStepRate = 0;

		// check if this is a per instance parameter
		if(((std::string)parameterDescription.SemanticName).find("_PER_INSTANCE") != std::string::npos) {
			elementDescription.InputSlot = 1; // assume instanced data comes from another slot
			elementDescription.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDescription.InstanceDataStepRate = 1;
		}

		// https://takinginitiative.net/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
		// determine DXGI format
		if(parameterDescription.Mask == 1) {
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDescription.Format = DXGI_FORMAT_R32_UINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDescription.Format = DXGI_FORMAT_R32_SINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDescription.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if(parameterDescription.Mask <= 3) {
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDescription.Format = DXGI_FORMAT_R32G32_UINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDescription.Format = DXGI_FORMAT_R32G32_SINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDescription.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if(parameterDescription.Mask <= 7) {
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDescription.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDescription.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if(parameterDescription.Mask <= 15) {
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDescription.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDescription.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		inputLayoutDescription.push_back(elementDescription);
	}

	result = device->CreateInputLayout(
		&inputLayoutDescription[0],
		(UINT)inputLayoutDescription.size(),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		inputLayout.GetAddressOf()
	);
}
