#include "GameObject.h"
#include "AssetManager.h"
using namespace DirectX;

GameObject::GameObject(Color color, bool translucent) {
	active = true;
	visible = true;
	toBeDeleted = false;
	this->translucent = translucent;

	AssetManager* assets = AssetManager::GetInstance();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;

	pixelShader = Assets->colorPS;
	this->sprite = nullptr;
	colorTint = color;
}

GameObject::GameObject(std::shared_ptr<Sprite> sprite, bool translucent) {
	active = true;
	visible = true;
	toBeDeleted = false;
	this->translucent = translucent;

	AssetManager* assets = AssetManager::GetInstance();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;

	pixelShader = Assets->imagePS;
	this->sprite = sprite;
	colorTint = Color::White;
	transform.SetScale(sprite->GetAspectRatio(), 1.0f);
}

void GameObject::SetZ(float z) {
	transform.SetZ(z);
	if(scene) scene->UpdateDrawOrder(this);
}

Transform* GameObject::GetTransform() {
	return &transform;
}

bool GameObject::IsTranslucent() {
	return translucent;
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
		pixelShader->SetTexture(sprite->GetResourceView());
	}

	vertexShader->SetShader();
	pixelShader->SetShader();
	mesh->Draw();
}
