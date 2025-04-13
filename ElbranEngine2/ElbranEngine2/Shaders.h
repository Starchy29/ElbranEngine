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

struct VertexShader {
	VSData* shader;
	ConstantBuffer constants;

	void Release();
};

struct GeometryShader {
	GSData* shader;
	ConstantBuffer constants;

	void Release();
};

struct PixelShader {
	PSData* shader;
	ConstantBuffer constants;

	void Release();
};

struct ComputeShader {
	CSData* shader;
	ConstantBuffer constants;
	unsigned int xGroupSize;
	unsigned int yGroupSize;
	unsigned int zGroupSize;

	void Release();
};