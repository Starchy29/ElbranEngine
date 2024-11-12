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
	colorTint = WHITE;
	transform.SetScale(sprite->GetAspectRatio(), 1.0f);
}

void GameObject::SetZ(float z) {
	transform.SetZ(z);
	if(scene) scene->UpdateDrawOrder(this);
	for(GameObject* child : children) {
		if(child->scene) child->scene->UpdateDrawOrder(this);;
	}
}

void GameObject::SetParent(GameObject* newParent) {
	if(parent != nullptr) {
		RemoveParent();
	}

	if(newParent == nullptr) {
		parent = nullptr;
		transform.parent = nullptr;
		return;
	}

	parent = newParent;
	transform.parent = &(newParent->transform);

	parent->children.push_back(this);
	parent->transform.children.push_back(&transform);
}

Transform* GameObject::GetTransform() {
	return &transform;
}

bool GameObject::IsTranslucent() {
	return translucent;
}

void GameObject::RemoveParent() {
	parent->children.remove(this);
	parent->transform.children.remove(&transform);
	parent = nullptr;
	transform.parent = nullptr;
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

void GameObject::Delete(bool keepChildren) {
	toBeDeleted = true;
	if(parent != nullptr && !parent->toBeDeleted) {
		RemoveParent();
	}

	if(keepChildren) {
		for(GameObject* child : children) {
			child->SetParent(nullptr);
		}
	} else {
		for(GameObject* child : children) {
			child->Delete();
		}
	}
}
