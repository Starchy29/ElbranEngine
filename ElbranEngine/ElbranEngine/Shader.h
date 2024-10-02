#pragma once
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <wrl/client.h>
#include <d3d11.h>
#include <stdexcept>
#include <d3dcompiler.h>

// base abstract class for all types of shaders
class Shader
{
public:
	Shader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

	virtual void SetShader() = 0;

protected:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	void LoadShader(std::wstring fileName); // must be called by subclass constructors
	virtual void CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) = 0;
};

