#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <string>

class Sprite
{
public:
	Sprite(std::wstring fileName);

	int GetPixelWidth();
	int GetPixelHeight();
	float GetAspectRatio();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResourceView();

protected:
	int pixelWidth;
	int pixelHeight;
	float aspectRatio;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView;
};

