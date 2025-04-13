#pragma once
#include "Math.h"

struct Transform {
	Vector2 position;
	Vector2 scale;
	float rotation;
	float zOrder;
	const Transform* parent;

	// AlignedRect GetArea() const;
	// void SetEdge
};
