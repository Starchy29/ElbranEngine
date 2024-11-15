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
	Scene(float cameraWidth, Color backgroundColor);
	Scene(float cameraWidth, std::shared_ptr<Sprite> backgroundImage);
	~Scene();

	Camera* GetCamera();

	void Update(float deltaTime);
	void Draw();
	void Join(GameObject* object);
	void UpdateDrawOrder(GameObject* sceneMember);

private:
	bool hasBackground;
	Color backColor;
	std::shared_ptr<Sprite> backImage;

	Camera* camera;
	std::vector<GameObject*> opaques;
	std::vector<GameObject*> translucents;

	inline void DrawBackground();
	void SortInto(GameObject* sceneMember, std::vector<GameObject*> & objectList);
};

