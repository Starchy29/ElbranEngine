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
	Transform* transform;
	const Matrix* worldMatrix;
	float viewWidth;

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

	Scene() {}
	void Initialize(uint16_t maxEntities, float cameraWidth);
	void Release();

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
	uint16_t entityCount;
	uint16_t maxEntities;
	Transform* transforms;
	Matrix* localMatrices;
	Matrix* worldMatrices;
	std::vector<uint16_t> openSlots;

	DynamicFixedList<IBehavior*> behaviors;
	DynamicFixedList<LightSource> lights;
	DynamicFixedList<IRenderer*> opaques;
	DynamicFixedList<IRenderer*> translucents;
	FixedList<IBehavior*, 10> toBeDeleted;

	ConstantBuffer projectionBuffer;
	ConstantBuffer lightInfoBuffer;
	ArrayBuffer lightsBuffer;
};

