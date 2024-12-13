#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class IPostProcess
{
public:
	virtual void Render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputTexture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> outputTexture) = 0;
};