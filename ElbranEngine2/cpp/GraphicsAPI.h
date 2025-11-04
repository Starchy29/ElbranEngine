#pragma once
#include "AppPointer.h"
#include "Common.h"
#include "Buffers.h"
#include "Shaders.h"
#include "IPostProcess.h"
#include "Math.h"
#include "FixedList.h"
#include <string>

#define OBJECT_CONSTANT_REGISTER 0 // shaders with per-object constant buffers should use register 0
#define MAX_POST_PROCESS_HELPER_TEXTURES 3

class Game;

enum class BlendState {
	None,
	AlphaBlend,
	Additive
};

enum class RenderPrimitive {
	Point,
	Line,
	Triangle
};

enum class ShaderStage {
	Vertex,
	Geometry,
	Pixel,
	Compute
};

enum class ShaderDataType {
	Structured,
	Float,
	Int,
	UInt,
	Float2,
	Int2,
	UInt2,
	Float3,
	Int3,
	UInt3,
	Float4,
	Int4,
	UInt4
};

struct Vertex {
	Vector2 position;
	Vector2 uv;
};

class GraphicsAPI
{
public:
	FixedList<IPostProcess*, 3> postProcesses;

	GraphicsAPI();
	virtual ~GraphicsAPI() {}
	virtual void Release() {}

	virtual bool IsFullscreen() const = 0;
	virtual void SetFullscreen(bool fullscreen) {}

	void Render(Game* game);
	void ApplyPostProcesses();
	RenderTarget* GetPostProcessHelper(uint8_t slot);

	UInt2 GetViewDimensions() const;
	UInt2 GetViewOffset() const;
	float GetViewAspectRatio() const;

	virtual VertexShader LoadVertexShader(std::wstring fileName) = 0;
	virtual GeometryShader LoadGeometryShader(std::wstring fileName) = 0;
	virtual PixelShader LoadPixelShader(std::wstring fileName) = 0;
	virtual ComputeShader LoadComputeShader(std::wstring fileName) = 0;

	virtual Texture2D CreateConstantTexture(uint32_t width, uint32_t height, uint8_t* textureData) = 0;
	virtual Sampler CreateDefaultSampler() = 0;
	virtual Mesh CreateMesh(const Vertex* vertices, uint16_t vertexCount, const uint32_t* indices, uint16_t indexCount, bool editable) = 0;
	virtual ConstantBuffer CreateConstantBuffer(uint32_t byteLength) = 0;
	virtual ArrayBuffer CreateArrayBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) = 0;
	virtual EditBuffer CreateEditBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) = 0;
	virtual OutputBuffer CreateOutputBuffer(ShaderDataType type, uint32_t elements, uint32_t structBytes = 0u) = 0;
	virtual RenderTarget CreateRenderTarget(uint32_t width, uint32_t height) = 0;
	virtual ComputeTexture CreateComputeTexture(uint32_t width, uint32_t height) = 0;
	
	virtual void WriteBuffer(const void* data, uint32_t byteLength, Buffer* buffer) = 0; // fails if the buffer was not created with cpu write access
	virtual void SetConstants(ShaderStage stage, const ConstantBuffer* buffer, uint8_t slot) = 0;
	virtual void SetArray(ShaderStage stage, const ArrayBuffer* buffer, uint8_t slot) = 0;
	virtual void SetTexture(ShaderStage stage, const Texture2D* texture, uint8_t slot) = 0;
	virtual void SetSampler(ShaderStage stage, Sampler* sampler, uint8_t slot) = 0;
	virtual void CopyTexture(Texture2D* source, Texture2D* destination) = 0;
	virtual void ClearMesh() = 0;

	virtual void SetVertexShader(const VertexShader* shader) = 0;
	virtual void SetGeometryShader(const GeometryShader* shader) = 0;
	virtual void SetPixelShader(const PixelShader* shader) = 0;
	void SetVertexShader(const VertexShader* shader, const void* constantInput, uint32_t inputBytes);
	void SetGeometryShader(const GeometryShader* shader, const void* constantInput, uint32_t inputBytes);
	void SetPixelShader(const PixelShader* shader, const void* constantInput, uint32_t inputBytes);

	virtual void SetBlendMode(BlendState mode) = 0;
	virtual void SetPrimitive(RenderPrimitive primitive) = 0;
	virtual void SetRenderTarget(const RenderTarget* renderTarget, bool useDepthStencil) = 0;
	virtual void ResetRenderTarget() = 0;

	virtual void DrawVertices(uint16_t numVertices) = 0;
	virtual void DrawMesh(const Mesh* mesh) = 0;
	void DrawFullscreen();

	// compute shader functions
	virtual void SetComputeTexture(const ComputeTexture* texture, uint8_t slot) = 0;
	virtual void SetEditBuffer(EditBuffer* buffer, uint8_t slot) = 0;
	virtual void SetOutputBuffer(OutputBuffer* buffer, uint8_t slot, const void* initialData = nullptr) = 0;
	virtual void ReadBuffer(const OutputBuffer* buffer, void* destination) = 0;
	virtual void RunComputeShader(const ComputeShader* shader, uint16_t xThreads, uint16_t yThreads, uint16_t zThreads = 1) = 0;

	// gpu memory release functions
	virtual void ReleaseShader(VertexShader* shader) = 0;
	virtual void ReleaseShader(GeometryShader* shader) = 0;
	virtual void ReleaseShader(PixelShader* shader) = 0;
	virtual void ReleaseShader(ComputeShader* shader) = 0;
	virtual void ReleaseSampler(Sampler* sampler) = 0;
	virtual void ReleaseTexture(Texture2D* texture) = 0;
	virtual void ReleaseRenderTarget(RenderTarget* texture) = 0;
	virtual void ReleaseComputeTexture(ComputeTexture* texture) = 0;
	virtual void ReleaseMesh(Mesh* mesh) = 0;
	virtual void ReleaseConstantBuffer(ConstantBuffer* buffer) = 0;
	virtual void ReleaseArrayBuffer(ArrayBuffer* buffer) = 0;
	virtual void ReleaseEditBuffer(EditBuffer* buffer) = 0;
	virtual void ReleaseOuputBuffer(OutputBuffer* buffer) = 0;

protected:
	UInt2 viewportDims;
	UInt2 viewportOffset;
	float viewAspectRatio;
	RenderTarget postProcessTargets[2];
	RenderTarget postProcessHelpers[MAX_POST_PROCESS_HELPER_TEXTURES];

	virtual void ClearBackBuffer() = 0;
	virtual void ClearDepthStencil() = 0;
	virtual void PresentSwapChain() = 0;
	virtual RenderTarget* GetBackBuffer() = 0;

private:
	bool postProcessed;
};

