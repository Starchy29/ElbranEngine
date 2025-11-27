#ifdef WINDOWS
#pragma once
#include <wrl/client.h>
#include "Common.h"
#include "LoadedFile.h"
#include "GraphicsData.h"

class ID3D11Device;
class ID3D11DeviceContext;
class IDXGISwapChain;
class ID3D11DepthStencilView;
class ID3D11DepthStencilState;
class ID3D11BlendState;
enum DXGI_FORMAT;

class DirectXAPI {
public:
    DirectXAPI(HWND windowHandle, const LoadedFile* sampleShader);
    ~DirectXAPI();

    bool IsFullscreen() const;
    void SetFullscreen(bool fullscreen);
    void Resize(UInt2 windowDims, UInt2 viewportDims, UInt2 viewportOffset);

    void DrawVertices(uint16_t numVertices);
    void DrawMesh(const Mesh* mesh);

    VertexShader CreateVertexShader(LoadedFile* shaderBlob) const;
    GeometryShader CreateGeometryShader(LoadedFile* shaderBlob) const;
    PixelShader CreatePixelShader(LoadedFile* shaderBlob) const;
    ComputeShader CreateComputeShader(LoadedFile* shaderBlob) const;

    Texture2D CreateConstantTexture(uint32_t width, uint32_t height, const uint8_t* textureData) const;
    Sampler CreateDefaultSampler() const;
    Mesh CreateMesh(const Vertex* vertices, uint16_t vertexCount, const uint32_t* indices, uint16_t indexCount, bool editable) const;
    ConstantBuffer CreateConstantBuffer(uint32_t byteLength) const;
    ArrayBuffer CreateArrayBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) const;
    EditBuffer CreateEditBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) const;
    OutputBuffer CreateOutputBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) const;
    RenderTarget CreateRenderTarget(uint32_t width, uint32_t height) const;
    ComputeTexture CreateComputeTexture(uint32_t width, uint32_t height) const;
    void CopyTexture(const Texture2D* source, Texture2D* destination) const;

    void SetEditBuffer(const EditBuffer* buffer, uint8_t slot);
    void WriteBuffer(const void* data, uint32_t byteLength, Buffer* buffer) const;
    void SetOutputBuffer(const OutputBuffer* buffer, uint8_t slot, const void* initialData);
    void ReadBuffer(const OutputBuffer* buffer, void* destination) const;

    void SetPrimitive(RenderPrimitive primitive);
    void SetBlendMode(BlendState mode);

    void SetConstants(ShaderStage stage, const ConstantBuffer* buffer, uint8_t slot);
    void SetArray(ShaderStage stage, const ArrayBuffer* buffer, uint8_t slot);
    void SetTexture(ShaderStage stage, const Texture2D* texture, uint8_t slot);
    void SetSampler(ShaderStage stage, const Sampler* sampler, uint8_t slot);

    void SetComputeTexture(const ComputeTexture* texture, uint8_t slot);
    void ClearMesh();

    void SetVertexShader(const VertexShader* shader);
    void SetGeometryShader(const GeometryShader* shader);
    void SetPixelShader(const PixelShader* shader);
    void RunComputeShader(const ComputeShader* shader, uint16_t xThreads, uint16_t yThreads, uint16_t zThreads = 1) const;
    void SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil);
    RenderTarget GetBackBuffer() const;
    void ClearDepthStencil();
    void ClearRenderTarget(RenderTarget* renderTarget);
    void PresentFrame();

    void ReleaseShader(VertexShader* shader);
    void ReleaseShader(GeometryShader* shader);
    void ReleaseShader(PixelShader* shader);
    void ReleaseShader(ComputeShader* shader);
    void ReleaseSampler(Sampler* sampler);
    void ReleaseTexture(Texture2D* texture);
    void ReleaseRenderTarget(RenderTarget* texture);
    void ReleaseComputeTexture(ComputeTexture* texture);
    void ReleaseMesh(Mesh* mesh);
    void ReleaseConstantBuffer(ConstantBuffer* buffer);
    void ReleaseArrayBuffer(ArrayBuffer* buffer);
    void ReleaseEditBuffer(EditBuffer* buffer);
    void ReleaseOuputBuffer(OutputBuffer* buffer);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultStencil;
    Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> additiveBlendState;

    TextureData* CreateTexture(uint32_t width, uint32_t height, bool renderTarget, bool computeWritable, const void* initialData = nullptr) const;
    Buffer* CreateIndexedBuffer(uint32_t elements, uint32_t elementBytes, bool structured, bool cpuWrite, bool gpuWrite) const;

    ConstantBuffer LoadConstantBuffer(const LoadedFile* shaderBlob) const;

    static DXGI_FORMAT FormatOf(ShaderDataType type);
    static uint32_t ByteLengthOf(ShaderDataType type);
};
#endif