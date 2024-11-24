#pragma once
#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "NewGame.h"

class DXCore
{
	friend class Application;

public:
	void SetAlphaBlend(bool enabled);

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const;
	DirectX::XMINT2 GetViewDimensions();
	DirectX::XMINT2 GetViewOffset();

	// prevent copying
	DXCore(const DXCore&) = delete;
	void operator=(const DXCore&) = delete;

private:
	D3D_FEATURE_LEVEL featureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;

	DirectX::XMINT2 viewportDims;
	DirectX::XMINT2 viewportShift;

	DXCore(HWND windowHandle, DirectX::XMINT2 windowDims, float viewAspectRatio, HRESULT* outResult);

	void Resize(DirectX::XMINT2 windowDims, float viewAspectRatio);
	void Render(NewGame* game);
};

