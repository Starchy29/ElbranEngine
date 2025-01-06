#pragma once
#include <memory>
#include "Transform.h"
#include "IRenderer.h"
#include <SpriteFont.h>

class Scene;
struct Color;
class Sprite;
class SpriteAtlas;
class Camera;
class IBehavior;
class IRenderer;

enum class RenderMode {
	Opaque,
	Translucent,
	Text,
	Light
};

enum class ObjectTag {
	Default,
	MenuButton
};

class GameObject {
	friend class Scene;

public:
	GameObject(float zCoord, RenderMode renderMode, IRenderer* renderer); // standard constructor
	GameObject(float zCoord, Color color, bool circle = false); // solid color
	GameObject(float zCoord, std::shared_ptr<Sprite> sprite, bool translucent); // sprite
	GameObject(float zCoord, std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color); // text box
	GameObject(float radius, float brightness, Color lightColor); // light
	GameObject(float zCoord, unsigned int maxParticles, bool translucent); // particle system
	virtual ~GameObject();

	virtual void Update(float deltaTime);
	virtual void Draw(Camera* camera);
	virtual void Delete(bool keepChildren = false);
	virtual GameObject* Clone() const;

	void SetZ(float z);
	void SetActive(bool active);
	void SetParent(GameObject* newParent);
	void AddBehavior(IBehavior* behavior);

	Transform* GetTransform();
	template<class RenderType> RenderType* GetRenderer() { return (RenderType*)renderer; }
	Scene* GetScene() const;
	bool IsActive() const;
	ObjectTag GetType() const;

protected:
	bool active;
	ObjectTag type;
	Transform transform;
	IRenderer* renderer;
	Scene* scene;
	GameObject* parent;
	std::list<GameObject*> children;
	std::vector<IBehavior*> behaviors;

	virtual GameObject* Copy() const;
	GameObject(const GameObject& original);

private:
	RenderMode renderMode;
	bool toBeDeleted;

	void RemoveParent();
};