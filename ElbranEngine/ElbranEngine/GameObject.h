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
#include "Color.h"

enum RenderMode {
	Opaque,
	Translucent,
	Text
};

enum Direction {
	Center,
	Up,
	Down,
	Left,
	Right
};

class GameObject {
	friend class Scene;

public:
	bool active;
	bool visible;
	bool flipX;
	bool flipY;
	
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;
	std::shared_ptr<Sprite> sprite;
	Color colorTint;

	GameObject(Scene* scene, RenderMode renderMode, float zCoord, Color color = Color::White);
	GameObject(Scene* scene, bool translucent, float zCoord, std::shared_ptr<Sprite> sprite);
	virtual ~GameObject();

	virtual void Update(float deltaTime);
	virtual void Draw(Camera* camera);
	void Delete(bool keepChildren = false);
	GameObject* Clone() const;

	void SetZ(float z);
	void SetParent(GameObject* newParent);
	void AddBehavior(IBehavior* behavior);

	Transform* GetTransform();
	RenderMode GetRenderMode() const;

protected:
	Transform transform;
	Scene* scene;
	GameObject* parent;
	std::list<GameObject*> children;
	std::vector<IBehavior*> behaviors;

	virtual GameObject* Copy() const; // override this to make deep copies

private:
	RenderMode renderMode;
	bool toBeDeleted;

	void RemoveParent();
};

