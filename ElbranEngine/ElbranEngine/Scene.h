#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "GameObject.h"
#include "Color.h"

class Scene
{
public:
	Scene(float cameraWidth, Color backgroundColor = Color::Clear);
	Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage);
	~Scene();

	Camera* GetCamera();

	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void Add(GameObject* object);
	void UpdateDrawOrder(GameObject* sceneMember);

protected:
	Color backColor;
	std::shared_ptr<Sprite> backImage;
	Camera* camera;
	
	virtual void DrawBackground();
	virtual void Remove(GameObject* removed) {}

private:
	std::vector<GameObject*> opaques;
	std::vector<GameObject*> translucents;
	std::vector<GameObject*> texts;
	void SortInto(GameObject* sceneMember, std::vector<GameObject*>* objectList);
};

