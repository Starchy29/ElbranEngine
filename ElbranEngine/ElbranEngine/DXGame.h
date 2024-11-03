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

#define GameInstance DXGame::GetInstance()

class DXGame
{
public:
	std::wstring exePath;

	static DXGame* GetInstance();
	static HRESULT Initialize(HINSTANCE hInst);

	~DXGame();

	HRESULT Run();
	LRESULT ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	float GetAspectRatio();
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState();
	void LoadTexture(std::wstring localPath, ID3D11ShaderResourceView** destination);

protected:
	virtual HRESULT LoadAssets();
	virtual void Update(float deltaTime);
	virtual void Draw();

private:
	static DXGame* instance;
	HINSTANCE hInstance;
	
	HWND windowHandle;
	D3D_FEATURE_LEVEL featureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device> dxDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	float aspectRatio;
	int windowWidth;
	int windowHeight;
	int viewWidth;
	int viewHeight;

	double performanceCountSeconds;
	__int64 lastPerfCount;

	// temp asset storage
	GameObject* testObject;
	Scene* sampleScene;

	DXGame(HINSTANCE hInst);

	HRESULT InitWindow();
	HRESULT InitDirectX();
	void Resize();
	void Render();
};

