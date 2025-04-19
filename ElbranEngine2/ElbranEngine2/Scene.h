#pragma once
#include "Transform.h"
#include "Buffers.h"
#include "IBehavior.h"
#include "Lights.h"
#include "FixedList.h"
#include <vector>
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

	void CreateTransform(Transform** outTransform, const Matrix** outMatrix);
	SpriteRenderer* AddSprite(Texture2D* sprite);

	void ReleaseTransform(Transform* transform);
	void RemoveSprite(SpriteRenderer* sprite);

private:
	int entityCount;
	Transform* transforms;
	Matrix* worldMatrices;

	std::vector<int> openSlots;
	FixedList<IBehavior*> behaviors;
	FixedList<SpriteRenderer*> sprites;
	FixedList<LightSource> lights;

	ConstantBuffer projectionBuffer;
	ConstantBuffer lightInfoBuffer;
	ArrayBuffer lightsBuffer;

	float t = 0.f; // TEMP
};

