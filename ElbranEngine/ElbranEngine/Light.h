#pragma once
#include "Color.h"
#include "Vector2.h"

#define MAX_LIGHTS_ONSCREEN 16

// these values match the buffers used in Lighting.hlsli
#define LIGHT_CONSTANTS_REGISTER 13
#define LIGHT_ARRAY_REGISTER 127

// this must match the struct in Lighting.hlsli
struct Light {
	Color color;
	Vector2 worldPosition;
	float radius;
	float brightness;
	float coneSize;
	float rotation;
};