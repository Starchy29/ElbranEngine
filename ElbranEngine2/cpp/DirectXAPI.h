#ifdef WINDOWS
#pragma once
#include "GraphicsAPI.h"
#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include "Common.h"
#include "LoadedFile.h"

class DirectXAPI :
    public GraphicsAPI
{
public:
    DirectXAPI(HWND windowHandle, Int2 windowDims, float viewAspectRatio, const LoadedFile* sampleShader);
    void Release() override;

    bool IsFullscreen() const override;
    void SetFullscreen(bool fullscreen) override;
    void Resize(Int2 windowDims, float viewAspectRatio);

    void DrawVertices(uint16_t numVertices) override;
    void DrawMesh(const Mesh* mesh) override;

    VertexShader LoadVertexShader(std::wstring fileName) override;
    GeometryShader LoadGeometryShader(std::wstring fileName) override;
    PixelShader LoadPixelShader(std::wstring fileName) override;
    ComputeShader LoadComputeShader(std::wstring fileName) override;

    Texture2D CreateConstantTexture(uint32_t width, uint32_t height, uint8_t* textureData) override;
    Sampler CreateDefaultSampler() override;
    Mesh CreateMesh(const Vertex* vertices, uint16_t vertexCount, const uint32_t* indices, uint16_t indexCount, bool editable) override;
    ConstantBuffer CreateConstantBuffer(uint32_t byteLength) override;
    ArrayBuffer CreateArrayBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) override;
    EditBuffer CreateEditBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) override;
    OutputBuffer CreateOutputBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) override;
    RenderTarget CreateRenderTarget(uint32_t width, uint32_t height) override;
    ComputeTexture CreateComputeTexture(uint32_t width, uint32_t height) override;
    void CopyTexture(Texture2D* source, Texture2D* destination) override;

    void SetEditBuffer(EditBuffer* buffer, uint8_t slot) override;
    void WriteBuffer(const void* data, uint32_t byteLength, Buffer* buffer) override;
    void SetOutputBuffer(OutputBuffer* buffer, uint8_t slot, const void* initialData) override;
    void ReadBuffer(const OutputBuffer* buffer, void* destination) override;

    void SetPrimitive(RenderPrimitive primitive) override;
    void SetBlendMode(BlendState mode) override;

    void SetConstants(ShaderStage stage, const ConstantBuffer* buffer, uint8_t slot) override;
    void SetArray(ShaderStage stage, const ArrayBuffer* buffer, uint8_t slot) override;
    void SetTexture(ShaderStage stage, const Texture2D* texture, uint8_t slot) override;
    void SetSampler(ShaderStage stage, Sampler* sampler, uint8_t slot) override;

    void SetComputeTexture(const ComputeTexture* texture, uint8_t slot) override;
    void ClearMesh() override;

    void SetVertexShader(const VertexShader* shader) override;
    void SetGeometryShader(const GeometryShader* shader) override;
    void SetPixelShader(const PixelShader* shader) override;
    void RunComputeShader(const ComputeShader* shader, uint16_t xThreads, uint16_t yThreads, uint16_t zThreads = 1) override;
    void SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil) override;

    void ReleaseShader(VertexShader* shader) override;
    void ReleaseShader(GeometryShader* shader) override;
    void ReleaseShader(PixelShader* shader) override;
    void ReleaseShader(ComputeShader* shader) override;
    void ReleaseSampler(Sampler* sampler) override;
    void ReleaseTexture(Texture2D* texture) override;
    void ReleaseRenderTarget(RenderTarget* texture) override;
    void ReleaseComputeTexture(ComputeTexture* texture) override;
    void ReleaseMesh(Mesh* mesh) override;
    void ReleaseConstantBuffer(ConstantBuffer* buffer) override;
    void ReleaseArrayBuffer(ArrayBuffer* buffer) override;
    void ReleaseEditBuffer(EditBuffer* buffer) override;
    void ReleaseOuputBuffer(OutputBuffer* buffer) override;

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

    TextureData* CreateTexture(uint32_t width, uint32_t height, bool renderTarget, bool computeWritable, void* initialData = nullptr);
    Buffer* CreateIndexedBuffer(uint32_t elements, uint32_t elementBytes, bool structured, bool cpuWrite, bool gpuWrite);

    ConstantBuffer LoadConstantBuffer(const LoadedFile* shaderBlob);

    DXGI_FORMAT FormatOf(ShaderDataType type);
    uint32_t ByteLengthOf(ShaderDataType type);
};
#endif