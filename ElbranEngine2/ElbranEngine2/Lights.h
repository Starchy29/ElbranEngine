#pragma once
#include "Math.h"
#include "Common.h"
#include "Transform.h"

// matches buffer in Lighting.hlsli
struct LightConstants {
	Color ambientLight;
	int lightCount;
};

// matches struct in Lighting.hlsli
struct LightData {
	Vector2 worldPosition;
	float rotation;

	Color color;
	float radius;
	float brightness;
	float coneSize;
};

struct LightSource {
	Transform* transform;
	Matrix const* worldMatrix;

	Color color;
	float radius;
	float brightness;
	float coneSize;
};