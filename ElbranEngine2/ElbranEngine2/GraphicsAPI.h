#pragma once
#include "Common.h"
#include "Buffers.h"
#include "Shaders.h"
#include "IPostProcess.h"
#include "Math.h"
#include <string>
#include <vector>

#define OBJECT_CONSTANT_REGISTER 0 // shaders with per-object constant buffers should use register 0
#define MAX_POST_PROCESS_HELPER_TEXTURES 2

class Game;

enum class BlendState {
	None,
	AlphaBlend,
	Additive
};

enum class ShaderStage {
	Vertex,
	Geometry,
	Pixel,
	Compute
};

struct Vertex {
	Vector2 position;
	Vector2 uv;
};

class GraphicsAPI
{
public:
	std::vector<IPostProcess*> postProcesses;

	GraphicsAPI();
	virtual ~GraphicsAPI();

	void Render(Game* game);
	void ApplyPostProcesses();
	const RenderTarget* GetPostProcessHelper(int slot);

	Int2 GetViewDimensions() const;
	Int2 GetViewOffset() const;
	float GetViewAspectRatio() const;

	//void StartTextBatch();
	// DirectX::SpriteBatch* GetSpriteBatch() const;
	//void FinishTextBatch();
	//void SetLights(const Light* lights, int numLights, const Color& ambientColor);
	//void SetFullscreen(bool fullscreen);

	virtual VertexShader LoadVertexShader(std::wstring directory, std::wstring fileName) = 0;
	virtual GeometryShader LoadGeometryShader(std::wstring directory, std::wstring fileName) = 0;
	virtual PixelShader LoadPixelShader(std::wstring directory, std::wstring fileName) = 0;
	virtual ComputeShader LoadComputeShader(std::wstring directory, std::wstring fileName) = 0;

	virtual Texture2D LoadSprite(std::wstring directory, std::wstring fileName) = 0;
	virtual Sampler CreateDefaultSampler() = 0;
	virtual Mesh CreateMesh(const Vertex* vertices, int vertexCount, const unsigned int* indices, int indexCount, bool editable) = 0;
	virtual ConstantBuffer CreateConstantBuffer(unsigned int byteLength) = 0;
	virtual ArrayBuffer CreateArrayBuffer(unsigned int elements, unsigned int elementBytes, bool structured) = 0;
	virtual EditBuffer CreateEditBuffer(unsigned int elements, unsigned int elementBytes, bool structured) = 0;
	virtual OutputBuffer CreateOutputBuffer(unsigned int elements, unsigned int elementBytes, bool structured) = 0;
	virtual RenderTarget CreateRenderTarget(unsigned int width, unsigned int height) = 0;
	virtual ComputeTexture CreateComputeTexture(unsigned int width, unsigned int height) = 0;
	
	virtual void WriteBuffer(const void* data, int byteLength, Buffer* buffer) = 0; // fails if the buffer was not created with cpu write access
	virtual void SetConstants(ShaderStage stage, const ConstantBuffer* buffer, unsigned int slot) = 0;
	virtual void SetArray(ShaderStage stage, const ArrayBuffer* buffer, unsigned int slot) = 0;
	virtual void SetTexture(ShaderStage stage, const Texture2D* texture, unsigned int slot) = 0;
	virtual void SetSampler(ShaderStage stage, Sampler* sampler, unsigned int slot) = 0;
	virtual void CopyTexture(Texture2D* source, Texture2D* destination) = 0;

	virtual void SetVertexShader(const VertexShader* shader) = 0;
	virtual void SetGeometryShader(const GeometryShader* shader) = 0;
	virtual void SetPixelShader(const PixelShader* shader) = 0;

	virtual void SetBlendMode(BlendState mode) = 0;
	virtual void SetRenderTarget(const RenderTarget* renderTarget) = 0;
	virtual void ResetRenderTarget() = 0;

	virtual void DrawVertices(unsigned int numVertices) = 0;
	virtual void DrawMesh(const Mesh* mesh) = 0;
	void DrawFullscreen();

	// compute shader functions
	virtual void SetComputeTexture(const ComputeTexture* texture, unsigned int slot) = 0;
	virtual void SetEditBuffer(EditBuffer* buffer, unsigned int slot) = 0;
	virtual void SetOutputBuffer(OutputBuffer* buffer, unsigned int slot, const void* initialData = nullptr) = 0;
	virtual void ReadBuffer(const OutputBuffer* buffer, void* destination) = 0;
	virtual void RunComputeShader(const ComputeShader* shader, unsigned int xThreads, unsigned int yThreads, unsigned int zThreads = 0) = 0;

protected:
	Int2 viewportDims;
	Int2 viewportOffset;
	float viewAspectRatio;
	RenderTarget postProcessTargets[2];
	RenderTarget postProcessHelpers[MAX_POST_PROCESS_HELPER_TEXTURES];

	virtual void ClearBackBuffer() = 0;
	virtual void ClearDepthStencil() = 0;
	virtual void PresentSwapChain() = 0;
	virtual RenderTarget GetBackBufferView() = 0;

private:
	bool postProcessed;
};

