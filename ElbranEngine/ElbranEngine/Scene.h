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
	bool hidden;

	Scene(float cameraWidth, Color backgroundColor = Color::Clear);
	Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage);
	~Scene();

	Camera* GetCamera();

	void Update(float deltaTime);
	void Draw();
	void Join(GameObject* object);
	void UpdateDrawOrder(GameObject* sceneMember);

private:
	Color backColor;
	std::shared_ptr<Sprite> backImage;

	Camera* camera;
	std::vector<GameObject*> opaques;
	std::vector<GameObject*> translucents;
	std::vector<GameObject*> texts;

	inline void DrawBackground();
	void SortInto(GameObject* sceneMember, std::vector<GameObject*> & objectList);
};

