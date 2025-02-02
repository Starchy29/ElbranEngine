#pragma once
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <wrl/client.h>
#include <d3d11.h>
#include <stdexcept>
#include <d3dcompiler.h>
#include <unordered_map>

struct ConstantBuffer {
	Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer;
	BYTE* localData;
	UINT size;
	UINT bindIndex;
	bool needsUpdate;
};

struct ConstantVariable {
	UINT bufferIndex;
	UINT byteOffset;
	UINT size;
};

// represents a Buffer<> or StucturedBuffer<>
struct ArrayBuffer {
	UINT elements;
	UINT elementSize;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
};

enum class ShaderDataType {
	Structured,
	Float,
	Int,
	UInt,
	Float2,
	Int2,
	UInt2,
	Float3,
	Int3,
	UInt3,
	Float4,
	Int4,
	UInt4
};

// base abstract class for all types of shaders
class Shader
{
public:
	static ArrayBuffer CreateArrayBuffer(UINT elements, UINT elementBytes, ShaderDataType type);
	static void WriteArray(const void* data, ArrayBuffer* destination);

	Shader(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	virtual ~Shader();

	void SetShader();
	void SetConstantVariable(std::string name, const void* data);
	void SetBool(std::string name, bool value);
	virtual void SetArrayBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view, int slot) = 0;

protected:
	static DXGI_FORMAT FormatOfType(ShaderDataType type);

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

