#pragma once
#include "Transform.h"

struct Camera {
	Transform* transform;
	const Matrix* worldMatrix;
	float viewWidth;

	float GetViewHeight() const;
	Vector2 GetWorldDimensions() const;
	AlignedRect GetViewArea() const;
};