#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Camera.h"
#include "Color.h"

class GameObject {
public:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	Color colorTint;

	GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
	Transform* GetTransform();
	void Draw(std::shared_ptr<Camera> camera);

private:
	Transform transform;
};

