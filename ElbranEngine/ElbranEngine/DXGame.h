// https://learn.microsoft.com/en-us/windows/win32/direct3dgetstarted/building-your-first-directx-app
#pragma once
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include <string>
#include <memory>
#include "VertexShader.h"
#include "PixelShader.h"
#include "Mesh.h"
#include "Color.h"
#include "GameObject.h"
#include "Scene.h"

class DXGame
{
public:
	std::wstring exePath;

	virtual ~DXGame();

	HRESULT Run();
	LRESULT ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void EnableAlpha();
	void DisableAlpha();

	float GetAspectRatio();
	DirectX::XMINT2 GetWindowDims();
	DirectX::XMINT2 GetViewportDims();
	DirectX::XMINT2 GetViewportShift();
	Microsoft::WRL::ComPtr<ID3D11Device> GetDXDevice();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDXContext();
	void LoadTexture(std::wstring localPath, ID3D11ShaderResourceView** destination);

protected:
	DXGame(HINSTANCE hInst);

	HINSTANCE hInstance;
	HWND windowHandle;

	HRESULT InitWindow();
	HRESULT InitDirectX();

	virtual HRESULT LoadAssets();
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;

private:
	D3D_FEATURE_LEVEL featureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device> dxDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;

	float aspectRatio;
	DirectX::XMINT2 windowDims;
	DirectX::XMINT2 viewportDims;
	DirectX::XMINT2 viewportShift;

	double performanceCountSeconds;
	__int64 lastPerfCount;

	void Resize();
	void Render();
};

