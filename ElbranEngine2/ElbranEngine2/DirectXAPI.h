#pragma once
#include "GraphicsAPI.h"
#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include "Common.h"

class DirectXAPI :
    public GraphicsAPI
{
public:
    DirectXAPI(HWND windowHandle, Int2 windowDims, float viewAspectRatio);
    ~DirectXAPI();

    void Resize(Int2 windowDims, float viewAspectRatio);

    void SetBlendMode(BlendState mode) override;

protected:
    void ClearRenderTarget() override;
    void ClearDepthStencil() override;
    void PresentSwapChain() override;

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultStencil;

    Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> additiveBlendState;
};

