#pragma once
#include <memory>
#include "Camera.h"
#include "GameObject.h"
#include "Color.h"
#include "Light.h"

class PixelShader;
struct ID3D11SamplerState;
class ObjectList;

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
	virtual void Remove(GameObject* removed);
	void UpdateDrawOrder(GameObject* sceneMember);

protected:
	Color backColor;
	std::shared_ptr<Sprite> backImage;
	Camera* camera;
	
	virtual void DrawBackground();

private:
	Light lights[MAX_LIGHTS_ONSCREEN];

	ObjectList* sceneMembers;
	std::vector<GameObject*> toBeDeleted;

	ObjectList* opaques;
	ObjectList* translucents;
	ObjectList* texts;
	ObjectList* lightObjects;
};

