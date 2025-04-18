#pragma once
#include "Transform.h"
#include "Buffers.h"
#include "IBehavior.h"
#include "Lights.h"
#include "FixedList.h"
#include "SpriteRenderer.h"

#define CAMERA_Z 0
#define CAMERA_DEPTH 100
#define MAX_LIGHTS_ONSCREEN 16

struct Camera {
	Vector2 position;
	float viewWidth;
	float rotation;
};

class Scene
{
public:
	Camera camera;
	Color ambientLight;
	Color backgroundColor;
	Texture2D* backgroundImage;

	Scene(unsigned int maxEntities, float cameraWidth);
	virtual ~Scene();

	virtual void Update(float deltaTime);
	virtual void Draw();

	SpriteRenderer* AddSprite(Texture2D* sprite);

	void RemoveSprite(SpriteRenderer* sprite);

private:
	int entityCount;
	Transform* transforms;
	Matrix* worldMatrices;

	FixedList<IBehavior*> behaviors;
	FixedList<SpriteRenderer*> sprites;
	FixedList<LightSource> lights;

	ConstantBuffer projectionBuffer;
	ConstantBuffer lightInfoBuffer;
	ArrayBuffer lightsBuffer;

	float t = 0.f;
	
};

