#pragma once
#include "Renderer.h"
#include "AppComponents.h"

struct Camera {
	static constexpr float MIN_Z = -100.f;
	static constexpr float DEPTH = 200.f;

	Transform* transform;
	const Matrix* worldMatrix;
	float viewWidth;

	inline float GetViewHeight() const { return viewWidth / ASPECT_RATIO; }
	inline Vector2 GetWorldDimensions() const { return Vector2(viewWidth, GetViewHeight()); }
	inline AlignedRect GetViewArea() const {
		ASSERT(transform->rotation == 0.f)
		return AlignedRect(transform->position, GetWorldDimensions());
	}
};

struct RenderGroup {
	Camera* camera;
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
	void AddCamera(Camera* camera);
};

