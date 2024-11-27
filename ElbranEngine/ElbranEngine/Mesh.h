#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

struct Vertex {
	DirectX::XMFLOAT2 ScreenPosition;
	DirectX::XMFLOAT2 UV;
};

class Mesh
{
public:
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Vertex* vertices, int numVertices, unsigned int* indices, int numIndices);
	void Draw();

	static void ClearLastDrawn(); // called when using the spritebatch

private:
	static Mesh* lastDrawn;

	Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int numIndices;
};

