#include "Mesh.h"

Mesh* Mesh::lastDrawn = nullptr;

Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Vertex* vertices, int numVertices, unsigned int* indices, int numIndices) {
	this->numIndices = numIndices;
	dxContext = context;

	// create vertex buffer
	D3D11_BUFFER_DESC vertexDescription = {};
	vertexDescription.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDescription.ByteWidth = sizeof(Vertex) * numVertices;
	vertexDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDescription.CPUAccessFlags = 0;
	vertexDescription.MiscFlags = 0;
	vertexDescription.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA rawVertexData = {};
	rawVertexData.pSysMem = vertices;

	device->CreateBuffer(&vertexDescription, &rawVertexData, vertexBuffer.GetAddressOf());

	// create index buffer
	D3D11_BUFFER_DESC indexDescription = {};
	indexDescription.Usage = D3D11_USAGE_IMMUTABLE;
	indexDescription.ByteWidth = sizeof(unsigned int) * numIndices;
	indexDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDescription.CPUAccessFlags = 0;
	indexDescription.MiscFlags = 0;
	indexDescription.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA rawIndexData = {};
	rawIndexData.pSysMem = indices;

	device->CreateBuffer(&indexDescription, &rawIndexData, indexBuffer.GetAddressOf());
}

void Mesh::Draw() {
	if(true || lastDrawn != this) {
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		dxContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		dxContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	lastDrawn = this;
	dxContext->DrawIndexed(numIndices, 0, 0);
}
