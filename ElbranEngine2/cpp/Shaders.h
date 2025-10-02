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
};

struct GeometryShader {
	GSData* shader;
	ConstantBuffer constants;
};

struct PixelShader {
	PSData* shader;
	ConstantBuffer constants;
};

struct ComputeShader {
	CSData* shader;
	ConstantBuffer constants;
	uint32_t xGroupSize;
	uint32_t yGroupSize;
	uint32_t zGroupSize;
};