#include "GameObject.h"
#include "DXGame.h"
#include "AssetManager.h"

GameObject::GameObject(Color color) {
	active = true;
	visible = true;
	toBeDeleted = false;

	AssetManager* assets = AssetManager::GetInstance();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;

	pixelShader = Assets->colorPS;
	this->sprite = nullptr;
	colorTint = color;
}

GameObject::GameObject(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sprite) {
	active = true;
	visible = true;
	toBeDeleted = false;

	AssetManager* assets = AssetManager::GetInstance();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;

	pixelShader = Assets->imagePS;
	this->sprite = sprite;
	colorTint = Color::White;
}

Transform* GameObject::GetTransform() {
	return &transform;
}

void GameObject::Update(float deltaTime) {}

void GameObject::Draw(Camera* camera) {
	DirectX::XMFLOAT4X4 worldMat = transform.GetWorldMatrix();
	DirectX::XMFLOAT4X4 viewMat = camera->GetView();
	DirectX::XMFLOAT4X4 projMat = camera->GetProjection();
	vertexShader->SetConstantVariable("worldTransform", &worldMat);
	vertexShader->SetConstantVariable("view", &viewMat);
	vertexShader->SetConstantVariable("projection", &projMat);

	pixelShader->SetConstantVariable("color", &colorTint);
	if(sprite != nullptr) {
		pixelShader->SetSampler(GameInstance->GetSamplerState());
		pixelShader->SetTexture(sprite);
	}

	vertexShader->SetShader();
	pixelShader->SetShader();

	mesh->Draw();
}
