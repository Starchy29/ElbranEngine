#pragma once
#include "Vector2.h"

namespace Tween {
	float Clamp(float value, float minimum, float maximum);
	float Lerp(float start, float end, float t);
	Vector2 Lerp(Vector2 start, Vector2 end, float t);
	float Arc(float t); // inverse parabola
}