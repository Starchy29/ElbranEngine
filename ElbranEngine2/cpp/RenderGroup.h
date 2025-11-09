#pragma once
#include "Renderer.h"
#include "Camera.h"

#define CAMERA_Z 0
#define CAMERA_DEPTH 100

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
	void ReleaseRenderers();

	void Draw() const;

	Transform* ReserveTransform(const Matrix** outMatrix = nullptr);
	Renderer* ReserveRenderer();
};

