#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "VertexShader.h"
#include "PixelShader.h"
#include "ComputeShader.h"
#include "GeometryShader.h"
#include "Mesh.h"
#include "Sprite.h"
#include <SpriteFont.h>

// container for all assets
class AssetManager
{
	friend class Application;

public:
	std::shared_ptr<Mesh> unitSquare;
	std::shared_ptr<Mesh> unitTriangle;

#pragma region shaders
	std::shared_ptr<VertexShader> cameraVS;
	std::shared_ptr<VertexShader> atlasVS;
	std::shared_ptr<VertexShader> backgroundVS;
	std::shared_ptr<VertexShader> particleVS;

	std::shared_ptr<PixelShader> imagePS;
	std::shared_ptr<PixelShader> hueSwapPS;
	std::shared_ptr<PixelShader> colorPS;
	std::shared_ptr<PixelShader> circlePS;
	std::shared_ptr<PixelShader> stretchyPS;

	std::shared_ptr<PixelShader> conSatValPP;
	std::shared_ptr<PixelShader> blurPP;
	std::shared_ptr<PixelShader> bloomFilterPP;
	std::shared_ptr<PixelShader> screenSumPP;

	std::shared_ptr<ComputeShader> brightnessSumCS;
	std::shared_ptr<ComputeShader> particleSpawnCS;
	std::shared_ptr<ComputeShader> particleMoveCS;

	std::shared_ptr<GeometryShader> particleGS;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSampler;
#pragma endregion

	std::shared_ptr<Sprite> testImage;
	std::shared_ptr<DirectX::DX11::SpriteFont> arial;

	void LoadTexture(std::wstring fileName, ID3D11ShaderResourceView** destination) const;
	std::shared_ptr<DirectX::DX11::SpriteFont> LoadFont(std::wstring fileName) const;

private:
	AssetManager();
	~AssetManager();
};

