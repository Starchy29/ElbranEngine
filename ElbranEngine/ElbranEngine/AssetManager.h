#pragma once
#include <wrl/client.h>
#include <d3d11.h>

#define Assets AssetManager::GetInstance()

// container for all assets to be globally accessible
class AssetManager
{
public:
	static AssetManager* GetInstance();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> testImage;

private:
	static AssetManager* instance;

	AssetManager();
	~AssetManager();
};

