#pragma once
#ifdef WINDOWS
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;

typedef ID3D11Texture2D TextureData;
typedef ID3D11ShaderResourceView ShaderResourceView;
typedef ID3D11RenderTargetView RenderTargetView;
typedef ID3D11UnorderedAccessView UnorderedAccessView;
#endif

struct Texture2D {
	TextureData* data;
	ShaderResourceView* srv;
	RenderTargetView* rtv;
	UnorderedAccessView* uav;

	enum class WriteAccess {
		None,
		RenderTarget,
		UnorderedAccess
	};
};