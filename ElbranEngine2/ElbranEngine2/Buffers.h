#pragma once
#ifdef WINDOWS
struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;

typedef ID3D11Buffer Buffer;
typedef ID3D11Texture2D TextureData;
typedef ID3D11ShaderResourceView ShaderResourceView;
typedef ID3D11RenderTargetView RenderTargetView;
typedef ID3D11UnorderedAccessView UnorderedAccessView;
#endif

class GraphicsAPI;

struct Texture2D {
	TextureData* data;
	ShaderResourceView* inputView;

	void Release(GraphicsAPI* graphics);
};

struct RenderTarget : Texture2D {
	RenderTargetView* outputView;

	void Release(GraphicsAPI* graphics);
};

struct ComputeTexture : Texture2D {
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
	unsigned int inputSlot;
	unsigned int byteLength;

	void Release(GraphicsAPI* graphics);
};

struct ArrayBuffer {
	Buffer* buffer;
	ShaderResourceView* view;

	void Release(GraphicsAPI* graphics);
};

// a gpu-writable buffer that gets bound to the render pipeline
struct EditBuffer : ArrayBuffer {
	UnorderedAccessView* computeView;

	void Release(GraphicsAPI* graphics);
};

// a gpu-writable buffer that the cpu can read back from
struct OutputBuffer {
	Buffer* gpuBuffer;
	Buffer* cpuBuffer;
	UnorderedAccessView* view;
	int byteLength;

	void Release(GraphicsAPI* graphics);
};