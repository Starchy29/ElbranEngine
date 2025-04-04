#ifdef WINDOWS
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

    //Texture2D CreateTexture(unsigned int width, unsigned int height, Texture2D::WriteAccess writability) override;
    RenderTarget CreateRenderTarget(unsigned int width, unsigned int height) override;
    ComputeTexture CreateComputeTexture(unsigned int width, unsigned int height) override;
    void CopyTexture(Texture2D* source, Texture2D* destination) override;
    void ReleaseData(void* gpuData) override;
    //Int2 DetermineDimensions(const Texture2D* texture) override;

    void WriteBuffer(const void* data, int byteLength, Buffer* buffer) override;
    void SetOutputBuffer(OutputBuffer* buffer, int slot, const void* initialData) override;
    void ReadBuffer(const OutputBuffer* buffer, void* destination) override;

    void SetBlendMode(BlendState mode) override;

    void SetRenderTarget(const RenderTarget* renderTarget) override;

protected:
    void ClearRenderTarget() override;
    void ClearDepthStencil() override;
    void PresentSwapChain() override;
    void ResetRenderTarget() override;
    RenderTarget GetBackBufferView() override;

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultStencil;

    Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> additiveBlendState;

    Texture2D* CreateTexture(unsigned int width, unsigned int height, bool renderTarget, bool computeWritable);
};
#endif