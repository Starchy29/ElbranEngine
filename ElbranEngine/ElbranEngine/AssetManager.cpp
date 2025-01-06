#include "AssetManager.h"
#include "Application.h"
#include <WICTextureLoader.h>

void AssetManager::LoadTexture(std::wstring fileName, ID3D11ShaderResourceView** destination) const {
	std::wstring fullPath = APP->ExePath() + L"Assets\\" + fileName;
	DXCore* dx = APP->Graphics();
	DirectX::CreateWICTextureFromFile(dx->GetDevice().Get(), dx->GetContext().Get(), fullPath.c_str(), nullptr, destination);
}

std::shared_ptr<DirectX::DX11::SpriteFont> AssetManager::LoadFont(std::wstring fileName) const {
	std::wstring fullPath = APP->ExePath() + L"Assets\\" + fileName;
	return std::make_shared<DirectX::DX11::SpriteFont>(APP->Graphics()->GetDevice().Get(), fullPath.c_str());
}

AssetManager::AssetManager() {
	DXCore* dx = APP->Graphics();
	Microsoft::WRL::ComPtr<ID3D11Device> dxDevice = dx->GetDevice();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> dxContext = dx->GetContext();

	// create default sampler state
	D3D11_SAMPLER_DESC samplerDescription = {};
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	dxDevice.Get()->CreateSamplerState(&samplerDescription, defaultSampler.GetAddressOf());

	// create unit square
	Vertex vertices[] = {
		{ DirectX::XMFLOAT2(-0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f) },
		{ DirectX::XMFLOAT2(-0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 0.0f) },
		{ DirectX::XMFLOAT2(0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f) }
	};

	unsigned int indices[] = {
		0, 1, 3, // clockwise winding order
		1, 2, 3
	};

	unitSquare = std::make_shared<Mesh>(dxDevice, dxContext, &vertices[0], 4, &indices[0], 6);

	// create unit triangle
	Vertex triVertices[] = {
		{ DirectX::XMFLOAT2(-0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT2(0.5f, 0.f), DirectX::XMFLOAT2(1.0f, 0.5f) },
		{ DirectX::XMFLOAT2(-0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f) }
	};

	unsigned int triIndices[] = { 0, 1, 2, }; // clockwise winding order

	unitTriangle = std::make_shared<Mesh>(dxDevice, dxContext, &triVertices[0], 3, &triIndices[0], 3);

	cameraVS = std::make_shared<VertexShader>(dxDevice, dxContext, L"CameraVS.cso");
	atlasVS = std::make_shared<VertexShader>(dxDevice, dxContext, L"TextureAtlasVS.cso");
	backgroundVS = std::make_shared<VertexShader>(dxDevice, dxContext, L"FullscreenVS.cso");
	particleVS = std::make_shared<VertexShader>(dxDevice, dxContext, L"ParticlePassVS.cso");

	imagePS = std::make_shared<PixelShader>(dxDevice, dxContext, L"TexturePS.cso");
	hueSwapPS = std::make_shared<PixelShader>(dxDevice, dxContext, L"HueSwapPS.cso");
	colorPS = std::make_shared<PixelShader>(dxDevice, dxContext, L"ColorFillPS.cso");
	circlePS = std::make_shared<PixelShader>(dxDevice, dxContext, L"CirclePS.cso");
	stretchyPS = std::make_shared<PixelShader>(dxDevice, dxContext, L"StretchyPS.cso");

	conSatValPP = std::make_shared<PixelShader>(dxDevice, dxContext, L"ConSatValPP.cso");
	blurPP = std::make_shared<PixelShader>(dxDevice, dxContext, L"BlurPP.cso");
	bloomFilterPP = std::make_shared<PixelShader>(dxDevice, dxContext, L"BloomFilterPP.cso");
	screenSumPP = std::make_shared<PixelShader>(dxDevice, dxContext, L"ScreenSumPP.cso");

	brightnessSumCS = std::make_shared<ComputeShader>(dxDevice, dxContext, L"BrightnessSumCS.cso");
	particleMoveCS = std::make_shared<ComputeShader>(dxDevice, dxContext, L"ParticleMovementCS.cso");
	particleSpawnCS = std::make_shared<ComputeShader>(dxDevice, dxContext, L"ParticleSpawnCS.cso");

	particleGS = std::make_shared<GeometryShader>(dxDevice, dxContext, L"ParticleQuadGS.cso");

	Vector2 oneVec = Vector2(1, 1);
	imagePS->SetConstantVariable("stretchFactor", &oneVec); // this var is for repeating textures, should default to (1,1)

	arial = LoadFont(L"Arial.spritefont");
	testImage = std::make_shared<Sprite>(L"temp sprite.png");
}

AssetManager::~AssetManager() {

}
