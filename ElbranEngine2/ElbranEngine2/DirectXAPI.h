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

    void DrawVertices(unsigned int numVertices) override;
    void DrawMesh(const Mesh* mesh) override;

    VertexShader LoadVertexShader(std::wstring fileName) override;
    GeometryShader LoadGeometryShader(std::wstring fileName) override;
    PixelShader LoadPixelShader(std::wstring fileName) override;
    ComputeShader LoadComputeShader(std::wstring fileName) override;

    Mesh CreateMesh(const Vertex* vertices, int vertexCount, const int* indices, int indexCount, bool editable) override;
    ArrayBuffer CreateArrayBuffer(unsigned int elements, unsigned int elementBytes, bool structured) override;
    EditBuffer CreateEditBuffer(unsigned int elements, unsigned int elementBytes, bool structured) override;
    OutputBuffer CreateOutputBuffer(unsigned int elements, unsigned int elementBytes, bool structured) override;
    RenderTarget CreateRenderTarget(unsigned int width, unsigned int height) override;
    ComputeTexture CreateComputeTexture(unsigned int width, unsigned int height) override;
    void CopyTexture(Texture2D* source, Texture2D* destination) override;
    void ReleaseData(void* gpuData) override;
    //Int2 DetermineDimensions(const Texture2D* texture) override;

    void SetEditBuffer(EditBuffer* buffer, unsigned int slot) override;
    void WriteBuffer(const void* data, int byteLength, Buffer* buffer) override;
    void SetOutputBuffer(OutputBuffer* buffer, unsigned int slot, const void* initialData) override;
    void ReadBuffer(const OutputBuffer* buffer, void* destination) override;

    void SetBlendMode(BlendState mode) override;

    void SetArray(ShaderStage stage, const ArrayBuffer* buffer, unsigned int slot) override;
    void SetTexture(ShaderStage stage, const Texture2D* texture, unsigned int slot) override;

    void SetComputeTexture(const ComputeTexture* texture, unsigned int slot) override;

    void SetVertexShader(const VertexShader* shader) override;
    void SetGeometryShader(const GeometryShader* shader) override;
    void SetPixelShader(const PixelShader* shader) override;
    void SetComputeShader(const ComputeShader* shader) override;
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

    TextureData* CreateTexture(unsigned int width, unsigned int height, bool renderTarget, bool computeWritable);
    Buffer* CreateIndexedBuffer(unsigned int elements, unsigned int elementBytes, bool structured, bool cpuWrite, bool gpuWrite);

    ID3DBlob* LoadShader(std::wstring fileName);
    void CreateBuffers(ID3DBlob* shaderBlob, Shader* output);
};
#endif