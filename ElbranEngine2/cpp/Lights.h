#pragma once
#include "Math.h"
#include "Common.h"
#include "Transform.h"

// matches buffer in Lighting.hlsli
struct LightConstants {
	Color ambientLight;
	int32_t lightCount;
};

// matches struct in Lighting.hlsli
struct LightData {
	Vector2 worldPosition;
	Vector2 direction;

	Color color;
	float radius;
	float brightness;
	Vector2 coneEdge;
};

struct LightSource {
	Transform* transform;
	Matrix const* worldMatrix;

	Color color;
	float radius;
	float brightness;
	float coneSize;
};