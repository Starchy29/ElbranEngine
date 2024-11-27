#include "GameObject.h"
#include "Application.h"
using namespace DirectX;

GameObject::GameObject(Scene* scene, RenderMode renderMode, float zCoord, Color color) {
	active = true;
	visible = true;
	flipX = false;
	flipY = false;
	toBeDeleted = false;
	parent = nullptr;
	children = std::list<GameObject*>();
	behaviors = std::vector<IBehavior*>();

	transform.SetZ(zCoord);
	this->renderMode = renderMode;
	this->scene = scene;
	scene->Join(this);

	const AssetManager* assets = APP->Assets();
	mesh = assets->unitSquare;
	vertexShader = assets->cameraVS;

	pixelShader = assets->colorPS;
	this->sprite = nullptr;
	colorTint = color;
}

GameObject::GameObject(Scene* scene, bool translucent, float zCoord, std::shared_ptr<Sprite> sprite)
	: GameObject(scene, translucent ? RenderMode::Translucent : RenderMode::Opaque, zCoord) 
{
	pixelShader = APP->Assets()->imagePS;
	this->sprite = sprite;
	transform.SetScale(sprite->GetAspectRatio(), 1.0f);
}

GameObject::~GameObject() {
	for(IBehavior* behavior : behaviors) {
		delete behavior;
	}
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

void GameObject::AddBehavior(IBehavior* behavior) {
	behaviors.push_back(behavior);
}

Transform* GameObject::GetTransform() {
	return &transform;
}

RenderMode GameObject::GetRenderMode() const {
	return renderMode;
}

GameObject* GameObject::Clone() const {
	GameObject* copy = Copy();

	for(GameObject* child : children) {
		GameObject* childCopy = child->Clone();
		childCopy->SetParent(copy);
	}

	return copy;
}

GameObject* GameObject::Copy() const {
	GameObject* copy = new GameObject(this->scene, this->renderMode, this->transform.z, this->colorTint);
	copy->active = active;
	copy->visible = visible;
	copy->flipX = flipX;
	copy->flipY = flipY;

	copy->mesh = mesh;
	copy->vertexShader = vertexShader;
	copy->pixelShader =  pixelShader;
	copy->sprite = sprite;

	copy->transform = transform;
	copy->toBeDeleted = toBeDeleted;

	// copy all behaviors
	for(IBehavior* behavior : behaviors) {
		copy->behaviors.push_back(behavior->Copy());
	}

	// parent and children set by Clone()

	return copy;
}

void GameObject::RemoveParent() {
	parent->children.remove(this);
	parent->transform.children.remove(&transform);
	parent = nullptr;
	transform.parent = nullptr;
}

void GameObject::Update(float deltaTime) {
	for(IBehavior* behavior : behaviors) {
		behavior->Update(deltaTime);
	}
}

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
	vertexShader->SetBool("flipX", flipX);
	vertexShader->SetBool("flipY", flipY);

	pixelShader->SetConstantVariable("color", &colorTint);
	if(sprite != nullptr) {
		pixelShader->SetSampler(APP->Assets()->defaultSampler);
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
