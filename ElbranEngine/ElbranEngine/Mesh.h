#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
public:
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Vertex* vertices, int numVertices, unsigned int* indices, int numIndices);
	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int numIndices;
};

