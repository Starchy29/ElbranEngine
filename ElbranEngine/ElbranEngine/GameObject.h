#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Color.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Sprite.h"
#include "Scene.h"

class GameObject {
	friend class Scene;

public:
	bool active;
	bool visible;
	
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;
	std::shared_ptr<Sprite> sprite;
	Color colorTint;

	GameObject(Color color, bool translucent);
	GameObject(std::shared_ptr<Sprite> sprite, bool translucent);

	virtual void Update(float deltaTime);
	virtual void Draw(Camera* camera);
	virtual void Delete(bool keepChildren = false);

	void SetZ(float z);
	void SetParent(GameObject* newParent);
	Transform* GetTransform();
	bool IsTranslucent();

protected:
	Transform transform;
	Scene* scene;
	GameObject* parent;
	std::list<GameObject*> children;

private:
	bool translucent; // true if this object ever has a pixel with alpha between 0-1 exclusive
	bool toBeDeleted;

	void RemoveParent();
};

