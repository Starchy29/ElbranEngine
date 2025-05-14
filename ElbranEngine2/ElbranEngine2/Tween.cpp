#include "Tween.h"
#include <cmath>

float Tween::Clamp(float value, float minimum, float maximum) {
	if(value > maximum) return maximum;
	return (value < minimum ? minimum : value);
}

float Tween::Lerp(float start, float end, float t) {
	return t * end + (1.0f - t) * start;
}

Vector2 Tween::Lerp(Vector2 start, Vector2 end, float t) {
	return t * end + (1.0f - t) * start;
}

float Tween::Sign(float number) {
	if(number > 0.f) return 1.0f;
	if(number < 0.f) return -1.0f;
	return 0.0f;
}

int Tween::Sign(int number) {
	if(number > 0) return 1;
	if(number < 0) return -1;
	return 0;
}

float Tween::SlowToFast(float t, float strength) {
	return powf(t, strength);
}

float Tween::FastToSlow(float t, float strength) {
	return powf(t, 1.0f / strength);
}

float Tween::EaseInOut(float t, float minSpeed) {
	// piecewise function whose derivative is half a quadratic parabola on either side of x=0.5, contrained for (0.5,0.5) being on the curve
	Vector2 shift = t < 0.5f ? Vector2::Zero : Vector2(1.f, 1.f);
	float base = t - shift.x;
	return (4.f - 4.f * minSpeed) * base * base * base + minSpeed * base + shift.y;
}

float Tween::SpeedInOut(float t, float minSpeed) {
	// the derivative of this function is a quadratic with a point at (0.5, minSpeed)
	return (4.f - 4.f * minSpeed) * t * t * t + (-6.f + 6.f * minSpeed) * t * t + (3.f - 2.f * minSpeed) * t;
}

float Tween::Rebound(float t) {
	return 1.0f - 2.0f * abs(t - 0.5f);
}

float Tween::Bounce(float t, float strength) {
	return powf(Rebound(t), strength);
}

float Tween::Arc(float t) {
	return -4 * t * t + 4 * t;
}
