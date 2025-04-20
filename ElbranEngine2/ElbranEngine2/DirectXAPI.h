#ifdef WINDOWS
#pragma once
#include "GraphicsAPI.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include "Common.h"

class DirectXAPI :
    public GraphicsAPI
{
public:
    DirectXAPI(HWND windowHandle, Int2 windowDims, float viewAspectRatio, std::wstring directory);
    ~DirectXAPI();

    void Resize(Int2 windowDims, float viewAspectRatio);

    void DrawVertices(unsigned int numVertices) override;
    void DrawMesh(const Mesh* mesh) override;

    VertexShader LoadVertexShader(std::wstring directory, std::wstring fileName) override;
    GeometryShader LoadGeometryShader(std::wstring directory, std::wstring fileName) override;
    PixelShader LoadPixelShader(std::wstring directory, std::wstring fileName) override;
    ComputeShader LoadComputeShader(std::wstring directory, std::wstring fileName) override;

    Texture2D LoadSprite(std::wstring directory, std::wstring fileName) override;
    Sampler CreateDefaultSampler() override;
    Mesh CreateMesh(const Vertex* vertices, int vertexCount, const unsigned int* indices, int indexCount, bool editable) override;
    ConstantBuffer CreateConstantBuffer(unsigned int byteLength) override;
    ArrayBuffer CreateArrayBuffer(ShaderDataType type, unsigned int elements, unsigned int structBytes = 0u) override;
    EditBuffer CreateEditBuffer(ShaderDataType type, unsigned int elements, unsigned int structBytes = 0u) override;
    OutputBuffer CreateOutputBuffer(ShaderDataType type, unsigned int elements, unsigned int structBytes = 0u) override;
    RenderTarget CreateRenderTarget(unsigned int width, unsigned int height) override;
    ComputeTexture CreateComputeTexture(unsigned int width, unsigned int height) override;
    void CopyTexture(Texture2D* source, Texture2D* destination) override;
    void UnbindTextures(ShaderStage stage) override;

    void SetEditBuffer(EditBuffer* buffer, unsigned int slot) override;
    void WriteBuffer(const void* data, int byteLength, Buffer* buffer) override;
    void SetOutputBuffer(OutputBuffer* buffer, unsigned int slot, const void* initialData) override;
    void ReadBuffer(const OutputBuffer* buffer, void* destination) override;

    void SetBlendMode(BlendState mode) override;

    void SetConstants(ShaderStage stage, const ConstantBuffer* buffer, unsigned int slot) override;
    void SetArray(ShaderStage stage, const ArrayBuffer* buffer, unsigned int slot) override;
    void SetTexture(ShaderStage stage, const Texture2D* texture, unsigned int slot) override;
    void SetSampler(ShaderStage stage, Sampler* sampler, unsigned int slot) override;

    void SetComputeTexture(const ComputeTexture* texture, unsigned int slot) override;

    void SetVertexShader(const VertexShader* shader) override;
    void SetGeometryShader(const GeometryShader* shader) override;
    void SetPixelShader(const PixelShader* shader) override;
    void RunComputeShader(const ComputeShader* shader, unsigned int xThreads, unsigned int yThreads, unsigned int zThreads = 1) override;
    void SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil) override;

protected:
    void ClearBackBuffer() override;
    void ClearDepthStencil() override;
    void PresentSwapChain() override;
    void ResetRenderTarget() override;
    RenderTarget* GetBackBuffer() override;

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    RenderTarget backBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultStencil;

    Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> additiveBlendState;

    TextureData* CreateTexture(unsigned int width, unsigned int height, bool renderTarget, bool computeWritable);
    Buffer* CreateIndexedBuffer(unsigned int elements, unsigned int elementBytes, bool structured, bool cpuWrite, bool gpuWrite);

    ID3DBlob* LoadShader(std::wstring directory, std::wstring fileName);
    ConstantBuffer LoadConstantBuffer(ID3DBlob* shaderBlob);

    DXGI_FORMAT FormatOf(ShaderDataType type);
    unsigned int ByteLengthOf(ShaderDataType type);
};
#endif