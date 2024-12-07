#include "GameObject.h"
#include "ColorRenderer.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "Scene.h"
using namespace DirectX;

GameObject::GameObject(float zCoord, RenderMode renderMode, IRenderer* renderer) {
	active = true;
	visible = true;
	toBeDeleted = false;
	parent = nullptr;
	children = std::list<GameObject*>();
	behaviors = std::vector<IBehavior*>();

	transform.SetZ(zCoord);
	this->renderMode = renderMode;
	this->renderer = renderer;
	type = ObjectTag::Default;
}

GameObject::GameObject(float zCoord, Color color, bool circle)
	: GameObject(zCoord, color.alpha < 1 ? RenderMode::Translucent : RenderMode::Opaque, new ColorRenderer(color, circle))
{ }

GameObject::GameObject(float zCoord, std::shared_ptr<Sprite> sprite, bool translucent)
	: GameObject(zCoord, translucent ? RenderMode::Translucent : RenderMode::Opaque, new SpriteRenderer(sprite))
{
	transform.SetScale(sprite->GetAspectRatio(), 1.0f);
}

GameObject::GameObject(float zCoord, std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color)
	: GameObject(zCoord, RenderMode::Text, new TextRenderer(text, font, color))
{ }

GameObject::~GameObject() {
	for(IBehavior* behavior : behaviors) {
		delete behavior;
	}
	delete renderer;
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

	if(scene == nullptr && newParent->scene != nullptr) {
		newParent->scene->Add(this);
	}
}

void GameObject::AddBehavior(IBehavior* behavior) {
	behaviors.push_back(behavior);
}

Transform* GameObject::GetTransform() {
	return &transform;
}

Scene* GameObject::GetScene() const {
	return scene;
}

GameObject* GameObject::Clone() const {
	GameObject* copy = Copy();
	if(scene != nullptr) {
		scene->Add(copy);
	}

	for(GameObject* child : children) {
		GameObject* childCopy = child->Clone();
		childCopy->SetParent(copy);
	}

	return copy;
}

GameObject* GameObject::Copy() const {
	return new GameObject(*this);
}

GameObject::GameObject(const GameObject& original) {
	active = original.active;
	visible = original.visible;
	type = original.type;

	transform = original.transform;
	if(original.renderer != nullptr) {
		renderer = original.renderer->Clone();
	}

	// scene joined in Clone()
	// parent and children set in Clone()
	parent = nullptr;

	// copy all behaviors
	for(IBehavior* behavior : original.behaviors) {
		behaviors.push_back(behavior->Clone());
	}

	renderMode = original.renderMode;
	toBeDeleted = original.toBeDeleted;
}

void GameObject::RemoveParent() {
	parent->children.remove(this);
	parent->transform.children.remove(&transform);
	parent = nullptr;
	transform.parent = nullptr;
}

void GameObject::Update(float deltaTime) {
	for(IBehavior* behavior : behaviors) {
		if(behavior->enabled) {
			behavior->Update(deltaTime);
		}
	}
}

void GameObject::Draw(Camera* camera) {
	if(renderer) {
		renderer->Draw(camera, transform);
	}
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
