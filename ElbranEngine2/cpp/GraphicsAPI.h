#pragma once
#include "Common.h"
#include "GraphicsData.h"

#ifdef WINDOWS
class DirectXAPI;
typedef DirectXAPI PlatformGraphics;
#endif

struct LoadedFile;
class AssetContainer;
struct PostProcess;

class GraphicsAPI {
public:
	ConstantBuffer projectionBuffer;
	ConstantBuffer lightInfoBuffer;
	ArrayBuffer lightsBuffer;
	OutputBuffer totalBrightnessBuffer;
	RenderTarget postProcessHelpers[MAX_POST_PROCESS_HELPER_TEXTURES];

	UInt2 viewportDims;
	UInt2 viewportOffset;

	GraphicsAPI() = default;
	void Initialize(PlatformGraphics* platformGraphics, UInt2 windowSize);
	void Release();
	void ResizeScreen(UInt2 windowSize);

	void ResetRenderTargets();
	void PresentFrame();
	void ApplyPostProcesses(const AssetContainer*, const PostProcess* postProcessSequence, uint8_t ppCount);

	bool IsFullscreen() const;
	void SetFullscreen(bool fullscreen);

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
	
	void WriteBuffer(const void* data, uint32_t byteLength, Buffer* buffer) const; // fails if the buffer was not created with cpu write access
	void CopyTexture(const Texture2D* source, Texture2D* destination) const;
	void SetConstants(ShaderStage stage, const ConstantBuffer* buffer, uint8_t slot);
	void SetArray(ShaderStage stage, const ArrayBuffer* buffer, uint8_t slot);
	void SetTexture(ShaderStage stage, const Texture2D* texture, uint8_t slot);
	void SetSampler(ShaderStage stage, const Sampler* sampler, uint8_t slot);
	void ClearMesh();

	void SetVertexShader(const VertexShader* shader, const void* constantInput = nullptr, uint32_t inputBytes = 0);
	void SetGeometryShader(const GeometryShader* shader, const void* constantInput = nullptr, uint32_t inputBytes = 0);
	void SetPixelShader(const PixelShader* shader, const void* constantInput = nullptr, uint32_t inputBytes = 0);

	void SetBlendMode(BlendState mode);
	void SetPrimitive(RenderPrimitive primitive);
	void SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil);

	void DrawVertices(uint16_t numVertices) const;
	void DrawMesh(const Mesh* mesh) const;
	void DrawFullscreen(const AssetContainer*);

	// compute shader functions
	void SetComputeTexture(const ComputeTexture* texture, uint8_t slot);
	void SetEditBuffer(const EditBuffer* buffer, uint8_t slot);
	void SetOutputBuffer(const OutputBuffer* buffer, uint8_t slot, const void* initialData = nullptr);
	void ReadBuffer(const OutputBuffer* buffer, void* destination) const;
	void RunComputeShader(const ComputeShader* shader, uint16_t xThreads, uint16_t yThreads, uint16_t zThreads = 1) const;

	// gpu memory release functions
	void ReleaseShader(VertexShader* shader) const;
	void ReleaseShader(GeometryShader* shader) const;
	void ReleaseShader(PixelShader* shader) const;
	void ReleaseShader(ComputeShader* shader) const;
	void ReleaseSampler(Sampler* sampler) const;
	void ReleaseTexture(Texture2D* texture) const;
	void ReleaseComputeTexture(ComputeTexture* texture) const;
	void ReleaseMesh(Mesh* mesh) const;
	void ReleaseConstantBuffer(ConstantBuffer* buffer) const;
	void ReleaseArrayBuffer(ArrayBuffer* buffer) const;
	void ReleaseEditBuffer(EditBuffer* buffer) const;
	void ReleaseOuputBuffer(OutputBuffer* buffer) const;

private:
	PlatformGraphics* platformGraphics;
	RenderTarget postProcessTargets[2];
	RenderTarget backBuffer;
	uint8_t renderTargetIndex;
};