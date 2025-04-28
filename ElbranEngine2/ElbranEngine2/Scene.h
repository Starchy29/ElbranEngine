#pragma once
#include "Transform.h"
#include "Buffers.h"
#include "IBehavior.h"
#include "Lights.h"
#include "FixedList.h"
#include <vector>
#include "IRenderer.h"

#define CAMERA_Z 0
#define CAMERA_DEPTH 100
#define MAX_LIGHTS_ONSCREEN 16

struct Camera {
	Vector2 position;
	float viewWidth;
	float rotation;

	float GetViewHeight() const;
	Vector2 GetWorldDimensions() const;
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

	void ReserveTransform(Transform** outTransform, const Matrix** outMatrix);
	void AddRenderer(IRenderer* renderer, bool translucent);
	void AddBehavior(IBehavior* behavior);
	LightSource* AddLight(Color color, float radius);

	void ReleaseTransform(Transform* transform);
	void RemoveRenderer(IRenderer* renderer);
	void RemoveBehavior(IBehavior* behavior);
	void RemoveLight(LightSource* light);

private:
	int entityCount;
	Transform* transforms;
	Matrix* worldMatrices;

	std::vector<int> openSlots;
	FixedList<IBehavior*> behaviors;
	FixedList<LightSource> lights;
	FixedList<IRenderer*> opaques;
	FixedList<IRenderer*> translucents;

	ConstantBuffer projectionBuffer;
	ConstantBuffer lightInfoBuffer;
	ArrayBuffer lightsBuffer;

	float t = 0.f; // TEMP
};

