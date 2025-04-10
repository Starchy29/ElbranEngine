#pragma once
#include "Buffers.h"

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
	ConstantBuffer* constantBuffers;

	void Release();
};

struct VertexShader : Shader {
	VSData* shader;

	void Release();
};

struct GeometryShader : Shader {
	GSData* shader;

	void Release();
};

struct PixelShader : Shader {
	PSData* shader;

	void Release();
};

struct ComputeShader : Shader {
	CSData* shader;
	unsigned int xGroupSize;
	unsigned int yGroupSize;
	unsigned int zGroupSize;

	void Release();
};