#include "Tween.h"
#include <stdlib.h>

float Tween::clamp(float value, float minimum, float maximum) {
	return (value > maximum ? maximum : value);
}
