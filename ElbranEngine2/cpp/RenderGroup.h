#pragma once
#include "Renderer.h"

#define CAMERA_Z 0
#define CAMERA_DEPTH 100

struct Camera {
	Transform* transform;
	const Matrix* worldMatrix;
	float viewWidth;

	float GetViewHeight() const;
	Vector2 GetWorldDimensions() const;
};

struct RenderGroup {
	Camera camera;
	Color ambientLight;
	Color backgroundColor;
	const Texture2D* backgroundImage;

	uint32_t transformCount;
	uint32_t transformCapacity;
	Transform* transforms;
	Matrix* worldMatrices;

	uint32_t rendererCount;
	uint32_t rendererCapacity;
	Renderer* renderers;

	RenderGroup() = default;
	void Initialize(Transform* transformBuffer, Matrix* worldMatrixBuffer, Renderer* rendererBuffer, uint32_t numTransforms, uint32_t numRenderers);

	void Draw() const;

	void ReserveTransform(Transform** outTransform, const Matrix** outMatrix);
	Renderer* ReserveRenderer();

	float GetLastGlobalZ(Transform* transform) const;
};

