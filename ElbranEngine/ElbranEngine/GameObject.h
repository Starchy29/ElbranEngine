#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Sprite.h"
#include "Scene.h"
#include "IBehavior.h"
#include "IRenderer.h"
#include "Color.h"
#include <SpriteFont.h>

enum RenderMode {
	Opaque,
	Translucent,
	Text
};

class GameObject {
	friend class Scene;

public:
	bool active;
	bool visible;

	GameObject(float zCoord, RenderMode renderMode);
	GameObject(float zCoord, Color color, bool circle = false);
	GameObject(float zCoord, std::shared_ptr<Sprite> sprite, bool translucent);
	GameObject(float zCoord, std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color);
	virtual ~GameObject();

	virtual void Update(float deltaTime);
	virtual void Draw(Camera* camera);
	void Delete(bool keepChildren = false);
	GameObject* Clone() const;

	void SetZ(float z);
	void SetParent(GameObject* newParent);
	void AddBehavior(IBehavior* behavior);

	Transform* GetTransform();
	template<class RenderType> RenderType* GetRenderer() { return (RenderType*)renderer; }
	Scene* GetScene() const;

protected:
	Transform transform;
	IRenderer* renderer;
	GameObject* parent;
	std::list<GameObject*> children;
	std::vector<IBehavior*> behaviors;

	virtual GameObject* Copy() const; // override this to make deep copies

private:
	RenderMode renderMode;
	bool toBeDeleted;
	Scene* scene;

	void RemoveParent();
};