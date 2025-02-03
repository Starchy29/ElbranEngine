#pragma once
#include "Vector2.h"

namespace Tween {
	float Clamp(float value, float minimum, float maximum);
	float Lerp(float start, float end, float t);
	Vector2 Lerp(Vector2 start, Vector2 end, float t);

	// domain and range of these are [0, 1]
	float SlowToFast(float t, float strength = 2.0f);
	float FastToSlow(float t, float strength = 2.0f);
	float EaseInOut(float t);

	float Rebound(float t); // upside down v
	float Bounce(float t);
	float Arc(float t); // inverse parabola
}