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
