#pragma once
#include "Math.h"

namespace Tween {
	float Clamp(float value, float minimum, float maximum);
	float Lerp(float start, float end, float t);
	Vector2 Lerp(Vector2 start, Vector2 end, float t);
	float Sign(float number);
	int Sign(int number);

	// domain and range of these are [0, 1]
	float SlowToFast(float t, float strength = 2.0f); // strength is 1-inf
	float FastToSlow(float t, float strength = 2.0f); // strength is 1-inf
	float EaseInOut(float t, float minSpeed = 0.f); // slow to fast to slow, minSpeed is 0-1
	float SpeedInOut(float t, float minSpeed = 0.f); // fast to slow to fast, minSpeed is 0-1
	float Rebound(float t); // upside down v
	float Bounce(float t, float strength = 2.0f); // rebound that accelerates towards the direction change, strength is 1-inf
	float Arc(float t); // inverse parabola
}