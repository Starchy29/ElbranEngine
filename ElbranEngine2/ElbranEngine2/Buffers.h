#pragma once
#ifdef WINDOWS
struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;

typedef ID3D11Buffer Buffer;
typedef ID3D11Texture2D Texture2D;
typedef ID3D11ShaderResourceView ShaderResourceView;
typedef ID3D11RenderTargetView RenderTargetView;
typedef ID3D11UnorderedAccessView UnorderedAccessView;
#endif

class GraphicsAPI;

struct Sprite {
	Texture2D* texture;
	ShaderResourceView* view;

	void Release(GraphicsAPI* graphics);
};

struct RenderTarget {
	Texture2D* texture;
	ShaderResourceView* inputView;
	RenderTargetView* outputView;

	void Release(GraphicsAPI* graphics);
};

struct ComputeTexture {
	Texture2D* texture;
	ShaderResourceView* inputView;
	UnorderedAccessView* outputView;

	void Release(GraphicsAPI* graphics);
};

struct Mesh {
	int indexCount;
	Buffer* vertices;
	Buffer* indices;

	void Release(GraphicsAPI* graphics);
};

struct ConstantBuffer {
	Buffer* buffer;
	int inputSlot;

	void Release(GraphicsAPI* graphics);
};

struct ArrayBuffer {
	Buffer* buffer;
	ShaderResourceView* view;

	void Release(GraphicsAPI* graphics);
};

struct OutputBuffer {
	Buffer* gpuBuffer;
	Buffer* cpuBuffer;
	UnorderedAccessView* view;
	int byteLength;

	void Release(GraphicsAPI* graphics);
};