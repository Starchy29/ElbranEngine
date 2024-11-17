#pragma once
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <wrl/client.h>
#include <d3d11.h>
#include <stdexcept>
#include <d3dcompiler.h>
#include <unordered_map>

struct ConstantVariable {
	UINT bufferIndex;
	UINT byteOffset;
	UINT size;
};

struct ConstantBuffer {
	Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer;
	BYTE* localData;
	UINT size;
	bool needsUpdate;
};

// base abstract class for all types of shaders
class Shader
{
public:
	Shader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~Shader();

	void SetShader();
	void SetConstantVariable(std::string name, void* data);
	void SetBool(std::string name, bool value);

protected:
	Microsoft::WRL::ComPtr<ID3D11Device> dxDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext;

	UINT cBuffCount;
	ConstantBuffer* constantBuffers;
	std::unordered_map<std::string, ConstantVariable> constantVars;

	void LoadShader(std::wstring fileName); // must be called by subclass constructors
	virtual void CreateShader(Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob) = 0;
	virtual void SetSpecificShader() = 0;
	virtual void SetConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer) = 0;
};

