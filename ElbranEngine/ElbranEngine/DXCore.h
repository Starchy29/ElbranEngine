#pragma once
#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include "PostProcessTexture.h"
#include "IPostProcess.h"
#include <vector>

#define MAX_POST_PROCESS_TEXTURES 1

class Game;

class DXCore
{
	friend class Application;

public:
	std::vector<IPostProcess*> postProcesses;

	void SetAlphaBlend(bool enabled);
	void StartTextBatch();
	void FinishTextBatch();
	void DrawScreen();

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const;
	DirectX::XMINT2 GetViewDimensions() const;
	DirectX::XMINT2 GetViewOffset() const;
	DirectX::SpriteBatch* GetSpriteBatch() const;
	PostProcessTexture* GetPostProcessTexture(int index);

	// prevent copying
	DXCore(const DXCore&) = delete;
	void operator=(const DXCore&) = delete;

private:
	D3D_FEATURE_LEVEL featureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;
	Microsoft::WRL::ComPtr < ID3D11DepthStencilState> defaultStencil;

	DirectX::SpriteBatch* spriteBatch;
	PostProcessTexture ppTex1;
	PostProcessTexture ppTex2;
	PostProcessTexture ppHelpers[MAX_POST_PROCESS_TEXTURES];

	DirectX::XMINT2 viewportDims;
	DirectX::XMINT2 viewportShift;

	DXCore(HWND windowHandle, DirectX::XMINT2 windowDims, float viewAspectRatio, HRESULT* outResult);
	~DXCore();

	void Resize(DirectX::XMINT2 windowDims, float viewAspectRatio);
	void Render(Game* game);
};

