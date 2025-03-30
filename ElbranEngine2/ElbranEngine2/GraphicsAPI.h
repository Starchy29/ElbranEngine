#pragma once
#include "Common.h"
#include "Texture2D.h"
#include "IPostProcess.h"
#include <string>
#include <vector>

#define MAX_POST_PROCESS_HELPER_TEXTURES 2

class Game;

enum class BlendState {
	None,
	AlphaBlend,
	Additive
};

class GraphicsAPI
{
public:
	std::vector<IPostProcess*> postProcesses;

	GraphicsAPI();
	virtual ~GraphicsAPI();

	void Render(Game* game);
	void ApplyPostProcesses();
	const Texture2D* GetPostProcessHelper(int slot);

	Int2 GetViewDimensions() const;
	Int2 GetViewOffset() const;

	//virtual Texture2D LoadTexture(std::wstring fileName) = 0;
	virtual Texture2D CreateTexture(unsigned int width, unsigned int height, Texture2D::WriteAccess writability) = 0;
	virtual void CopyTexture(const Texture2D* source, Texture2D* destination) = 0;
	virtual void ReleaseTexture(Texture2D* texture) = 0;
	//virtual Int2 DetermineDimensions(const Texture2D* texture) = 0;

	virtual void SetBlendMode(BlendState mode) = 0;
	//void StartTextBatch();
	//void FinishTextBatch();
	void DrawFullscreen();
	void DrawIndices(int numIndices);
	//void SetLights(const Light* lights, int numLights, const Color& ambientColor);

	void SetFullscreen(bool fullscreen);
	
	//DirectX::SpriteBatch* GetSpriteBatch() const;
	//PostProcessTexture* GetPostProcessTexture(int index);

	void SetVertexBuffer();
	void SetIndexBuffer();
	virtual void SetRenderTarget(const Texture2D* renderTarget) = 0;
	virtual void ResetRenderTarget() = 0;

protected:
	Int2 viewportDims;
	Int2 viewportOffset;
	Texture2D postProcessTargets[2];
	Texture2D postProcessHelpers[MAX_POST_PROCESS_HELPER_TEXTURES];

	virtual void ClearRenderTarget() = 0;
	virtual void ClearDepthStencil() = 0;
	virtual void PresentSwapChain() = 0;
	virtual Texture2D GetBackBufferView() = 0;

private:
	bool postProcessed;
};

