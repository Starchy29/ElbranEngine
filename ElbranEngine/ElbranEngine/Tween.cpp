#include "Tween.h"

float Tween::Clamp(float value, float minimum, float maximum) {
	if(value > maximum) return maximum;
	return (value < minimum ? minimum : value);
}

float Tween::Lerp(float start, float end, float t) {
	return start * (1.0f - t) + end * t;
}

Vector2 Tween::Lerp(Vector2 start, Vector2 end, float t) {
	return start * (1.0f - t) + end * t;
}

float Tween::SlowToFast(float t, float strength) {
	return (float)pow(t, strength);
}

float Tween::FastToSlow(float t, float strength) {
	return (float)pow(t, 1.0f / strength);
}

float Tween::EaseInOut(float t) {
	return t < 0.5 ? 
		4.f * t * t * t :
		1.f - (float)pow(-2.f * t + 2.f, 3.f) / 2.f;
}

float Tween::Arc(float t) {
	return -4 * t * t + 4 * t;
}
