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

struct VertexShader {
	VSData* shader;
};

struct GeometryShader {
	GSData* shader;
};

struct PixelShader {
	PSData* shader;
};

struct ComputeShader {
	CSData* shader;
};