#pragma once
#include "Common.h"

class Game;

enum class BlendState {
	None,
	AlphaBlend,
	Additive
};

class GraphicsAPI
{
public:
	GraphicsAPI();
	virtual ~GraphicsAPI();

	void Render(Game* game);

	Int2 GetViewDimensions() const;
	Int2 GetViewOffset() const;

	virtual void SetBlendMode(BlendState mode) = 0;
	void StartTextBatch();
	void FinishTextBatch();
	void DrawFullscreen();
	void DrawIndices(int numIndices);
	void ApplyPostProcesses();
	//void SetLights(const Light* lights, int numLights, const Color& ambientColor);

	void SetFullscreen(bool fullscreen);
	
	//DirectX::SpriteBatch* GetSpriteBatch() const;
	//PostProcessTexture* GetPostProcessTexture(int index);

	void SetVertexBuffer();
	void SetIndexBuffer();
	void SetRenderTarget();

protected:
	Int2 viewportDims;
	Int2 viewportOffset;

	virtual void ClearRenderTarget() = 0;
	virtual void ClearDepthStencil() = 0;
	virtual void PresentSwapChain() = 0;

private:
	bool postProcessed;
};

