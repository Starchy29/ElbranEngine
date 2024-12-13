#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class PostProcessTexture
{
public:

	void Resize();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResource();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTarget();

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
};

