#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "GameObject.h"

class Scene
{
public:
	bool paused;
	bool hidden;

	Scene(float cameraWidth);
	~Scene();

	Camera* GetCamera();

	void Update(float deltaTime);
	void Draw();
	void AddObject(GameObject* object);

private:
	Camera* camera;
	std::vector<GameObject*> objects;
};

