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
#include "Color.h"
#include "Vector2.h"
#include "Shader.h"

#define MAX_POST_PROCESS_TEXTURES 3
#define MAX_LIGHTS_ONSCREEN 16

// these values match the buffers used in Lighting.hlsli
#define LIGHT_CONSTANTS_REGISTER 13
#define LIGHT_ARRAY_REGISTER 127

// this must match the struct in Lighting.hlsli
struct Light {
	Color color;
	Vector2 worldPosition;
	float radius;
	float brightness;
	float coneSize;
	float rotation;
};

enum class BlendState {
	None,
	AlphaBlend,
	Additive
};

class Game;

class DXCore
{
	friend class Application;

public:
	std::vector<IPostProcess*> postProcesses;

	void SetBlendMode(BlendState mode);
	void StartTextBatch();
	void FinishTextBatch();
	void DrawScreen();
	void RunPostProcesses();
	void SetLights(const Light* lights, int numLights, const Color& ambientColor);

	void SetFullscreen(bool fullscreen);
	bool IsFullscreen() const;

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const;
	DirectX::XMUINT2 GetViewDimensions() const;
	DirectX::XMUINT2 GetViewOffset() const;
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
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultStencil;

	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> additiveBlendState;

	DirectX::SpriteBatch* spriteBatch;
	PostProcessTexture ppTex1;
	PostProcessTexture ppTex2;
	PostProcessTexture ppHelpers[MAX_POST_PROCESS_TEXTURES];

	DirectX::XMUINT2 viewportDims;
	DirectX::XMUINT2 viewportShift;

	bool postProcessed;

	// light buffers
	ArrayBuffer lightBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;

	DXCore(HWND windowHandle, DirectX::XMINT2 windowDims, float viewAspectRatio, HRESULT* outResult);
	~DXCore();

	void SetupLighting();

	void Resize(DirectX::XMINT2 windowDims, float viewAspectRatio);
	void Render(Game* game);
};

