#include "Tween.h"

float Tween::SpeedInOut(float t, float minSpeed) {
	// the derivative of this function is a quadratic with a point at (0, minSpeed)
	return (4.f - 4.f * minSpeed) * t * t * t + (-6.f + 6.f * minSpeed) * t * t + (3.f - 2.f * minSpeed) * t;
}
