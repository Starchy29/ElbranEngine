#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "GameObject.h"
#include "Color.h"
#include "DXCore.h"

class PixelShader;
struct ID3D11SamplerState;

class Scene
{
public:
	Color ambientLight;

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
	DXCore* directX;
	std::shared_ptr<PixelShader> imageShader;
	std::shared_ptr<PixelShader> colorShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSampler;

	Light lights[MAX_LIGHTS_ONSCREEN];

	std::vector<GameObject*> opaques;
	std::vector<GameObject*> translucents;
	std::vector<GameObject*> texts;
	std::vector<GameObject*> lightObjects;

	void SortInto(GameObject* sceneMember, std::vector<GameObject*>* objectList);
};

