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
	Scene(float cameraWidth, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> backgroundImage);
	~Scene();

	Camera* GetCamera();

	void Update(float deltaTime);
	void Draw();
	void AddObject(GameObject* object);

private:
	bool hasBackground;
	Color backColor;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> backImage;

	Camera* camera;
	std::vector<GameObject*> objects;
};

