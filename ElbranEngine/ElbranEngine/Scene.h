#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "GameObject.h"
#include "Color.h"

class Scene
{
public:
	bool paused;

	Scene(float cameraWidth, Color backgroundColor = Color::Clear);
	Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage);
	~Scene();

	Camera* GetCamera();

	virtual void Update(float deltaTime);
	virtual void Draw();
	void Add(GameObject* object);
	void UpdateDrawOrder(GameObject* sceneMember);

private:
	Color backColor;
	std::shared_ptr<Sprite> backImage;

	Camera* camera;
	std::vector<GameObject*> opaques;
	std::vector<GameObject*> translucents;
	std::vector<GameObject*> texts;

	virtual void DrawBackground();
	void SortInto(GameObject* sceneMember, std::vector<GameObject*>* objectList);
};

