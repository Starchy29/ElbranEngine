#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "VertexShader.h"
#include "PixelShader.h"
#include "Mesh.h"
#include "Sprite.h"

#define Assets AssetManager::GetInstance()

// container for all assets to be globally accessible
class AssetManager
{
public:
	static AssetManager* GetInstance();

	std::shared_ptr<Mesh> unitSquare;

#pragma region shaders
	std::shared_ptr<VertexShader> cameraVS;
	std::shared_ptr<VertexShader> backgroundVS;
	std::shared_ptr<PixelShader> imagePS;
	std::shared_ptr<PixelShader> colorPS;
	std::shared_ptr<PixelShader> circlePS;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSampler;
#pragma endregion

	std::shared_ptr<Sprite> testImage;

private:
	static AssetManager* instance;

	AssetManager();
	~AssetManager();
};

