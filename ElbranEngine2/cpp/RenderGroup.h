#pragma once
#include "Renderer.h"
#include "AppComponents.h"

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
	void Initialize(uint32_t maxRenderers, uint32_t extraTransforms = 0);
	void Release(const GraphicsAPI*);

	void Draw(DrawComponents);

	Transform* ReserveTransform(const Matrix** outMatrix = nullptr);
	Renderer* ReserveRenderer();
};

