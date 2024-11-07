#include "GameObject.h"
#include "AssetManager.h"
using namespace DirectX;

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
	XMFLOAT4X4 worldMat = transform.GetWorldMatrix();
	XMFLOAT4X4 viewMat = camera->GetView();
	XMFLOAT4X4 projMat = camera->GetProjection();
	XMMATRIX product = XMLoadFloat4x4(&worldMat);
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&viewMat));
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&projMat));
	XMFLOAT4X4 worldViewProj;
	XMStoreFloat4x4(&worldViewProj, product);
	vertexShader->SetConstantVariable("worldViewProj", &worldViewProj);

	pixelShader->SetConstantVariable("color", &colorTint);
	if(sprite != nullptr) {
		pixelShader->SetSampler(Assets->defaultSampler);
		pixelShader->SetTexture(sprite);
	}

	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();
}
