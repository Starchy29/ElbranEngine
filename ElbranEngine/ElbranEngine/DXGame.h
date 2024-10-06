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

#define GameInstance DXGame::GetInstance()

class DXGame
{
public:
	std::wstring exePath;
	std::shared_ptr<Mesh> unitSquare;

	static DXGame* GetInstance();
	static HRESULT Initialize(HINSTANCE hInst);

	HRESULT Run();
	LRESULT ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual HRESULT LoadAssets();
	virtual void Update(float deltaTime);
	virtual void Render();

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

	int windowWidth;
	int windowHeight;

	// temp shader storage
	std::shared_ptr<VertexShader> defaultVS;
	std::shared_ptr<PixelShader> defaultPS;
	Microsoft::WRL::ComPtr<ID3D11Buffer> colorCBuffer;
	Color testColor;

	DXGame(HINSTANCE hInst);

	HRESULT InitWindow();
	HRESULT InitDirectX();
	void Resize();
};

