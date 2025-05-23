#pragma once
#ifdef WINDOWS
struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;

typedef ID3D11Buffer Buffer;
typedef ID3D11Texture2D TextureData;
typedef ID3D11SamplerState SamplerState;
typedef ID3D11ShaderResourceView ShaderResourceView;
typedef ID3D11RenderTargetView RenderTargetView;
typedef ID3D11UnorderedAccessView UnorderedAccessView;
#endif

struct Sampler {
	SamplerState* state;
};

struct Texture2D {
	TextureData* data;
	ShaderResourceView* inputView;
	float aspectRatio;
};

struct SpriteSheet {
	Texture2D texture;
	unsigned int rows;
	unsigned int cols;
	unsigned int spriteCount;
	float SpriteAspectRatio() const { return texture.aspectRatio * rows / cols; }
};

struct RenderTarget : Texture2D {
	RenderTargetView* outputView;
};

struct ComputeTexture : Texture2D {
	UnorderedAccessView* outputView;
};

struct Mesh {
	int indexCount;
	Buffer* vertices;
	Buffer* indices;
};

struct ConstantBuffer {
	Buffer* data;
};

struct ArrayBuffer {
	Buffer* buffer;
	ShaderResourceView* view;
};

// a gpu-writable buffer that gets bound to the render pipeline
struct EditBuffer : ArrayBuffer {
	UnorderedAccessView* computeView;
};

// a gpu-writable buffer that the cpu can read back from
struct OutputBuffer {
	Buffer* gpuBuffer;
	Buffer* cpuBuffer;
	UnorderedAccessView* view;
	int byteLength;
};