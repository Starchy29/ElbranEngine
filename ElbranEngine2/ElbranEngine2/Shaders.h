#pragma once

#ifdef WINDOWS
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
struct ID3D11ComputeShader;

typedef ID3D11VertexShader VSData;
typedef ID3D11GeometryShader GSData;
typedef ID3D11PixelShader PSData;
typedef ID3D11ComputeShader CSData;
#endif

struct Shader {
	int numConstBuffers;
	int numArrayBuffers;
	ConstantBuffer* constantBuffers;
	ArrayBuffer* arrayBuffers;
};

struct VertexShader : Shader {
	VSData* shader;
};

struct GeometryShader : Shader {
	GSData* shader;
};

struct PixelShader : Shader {
	PSData* shader;
};

struct ComputeShader : Shader {
	CSData* shader;
};