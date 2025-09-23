#pragma once
#include "Transform.h"
#include "Buffers.h"
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
	const Texture2D* backgroundImage;

	Scene() {}
	void Initialize(uint32_t maxTransforms, uint32_t maxRenderers, uint32_t maxLights, float cameraWidth);
	void Release();

	void Draw();

	void ReserveTransform(Transform** outTransform, const Matrix** outMatrix);
	void AddRenderer(IRenderer* renderer, bool translucent);
	LightSource* AddLight(Color color, float radius);

private:
	uint32_t transformSize;
	uint32_t transformCapacity;
	Transform* transforms;
	Matrix* localMatrices;
	Matrix* worldMatrices;

	DynamicFixedList<LightSource> lights;
	DynamicFixedList<IRenderer*> opaques;
	DynamicFixedList<IRenderer*> translucents;

	ConstantBuffer projectionBuffer;
	ConstantBuffer lightInfoBuffer;
	ArrayBuffer lightsBuffer;
};

