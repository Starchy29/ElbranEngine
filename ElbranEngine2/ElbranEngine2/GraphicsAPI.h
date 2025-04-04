#pragma once
#include "Common.h"
#include "Buffers.h"
#include "Shaders.h"
#include "IPostProcess.h"
#include "Math.h"
#include <string>
#include <vector>

#define MAX_POST_PROCESS_HELPER_TEXTURES 2

class Game;

enum class BlendState {
	None,
	AlphaBlend,
	Additive
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

	//virtual Sprite LoadSprite(std::wstring fileName) = 0;
	virtual RenderTarget CreateRenderTarget(unsigned int width, unsigned int height) = 0;
	virtual ComputeTexture CreateComputeTexture(unsigned int width, unsigned int height) = 0;
	virtual void CopyTexture(Texture2D* source, Texture2D* destination) = 0;
	virtual void ReleaseData(void* gpuData) = 0;
	//virtual Int2 DetermineDimensions(const Texture2D* texture) = 0;

	virtual void SetBlendMode(BlendState mode) = 0;
	//void StartTextBatch();
	// DirectX::SpriteBatch* GetSpriteBatch() const;
	//void FinishTextBatch();
	//void DrawVertices(unsigned int numVertices);
	//void DrawFullscreen();
	//void DrawSquare();
	//void DrawMesh(const Mesh* mesh);
	//void SetLights(const Light* lights, int numLights, const Color& ambientColor);
	//void SetFullscreen(bool fullscreen);

	//virtual Mesh CreateMesh(const Vertex* vertices, int vertexCount, const int* indices, int indexCount, bool editable);

	//virtual OutputBuffer CreateOutputArrayBuffer(bool cpuAccessible);
	virtual void WriteBuffer(const void* data, int byteLength, Buffer* buffer) = 0; // fails if the buffer was not created with cpu write access
	virtual void SetOutputBuffer(OutputBuffer* buffer, int slot, const void* initialData) = 0;
	virtual void ReadBuffer(const OutputBuffer* buffer, void* destination) = 0;
	
	//virtual VertexShader LoadVertexShader(std::wstring fileName);
	//virtual GeometryShader LoadGeometryShader(std::wstring fileName);
	//virtual PixelShader LoadPixelShader(std::wstring fileName);
	//virtual ComputeShader LoadComputeShader(std::wstring fileName);

	//virtual void SetVertexShader(const VertexShader* shader);
	//virtual void SetGeometryShader(const GeometryShader* shader);
	//virtual void SetPixelShader(const PixelShader* shader);
	//virtual void SetComputeShader(const ComputeShader* shader);

	virtual void SetRenderTarget(const RenderTarget* renderTarget) = 0;
	virtual void ResetRenderTarget() = 0;

protected:
	Int2 viewportDims;
	Int2 viewportOffset;
	RenderTarget postProcessTargets[2];
	RenderTarget postProcessHelpers[MAX_POST_PROCESS_HELPER_TEXTURES];

	virtual void ClearRenderTarget() = 0;
	virtual void ClearDepthStencil() = 0;
	virtual void PresentSwapChain() = 0;
	virtual RenderTarget GetBackBufferView() = 0;

private:
	bool postProcessed;
};

