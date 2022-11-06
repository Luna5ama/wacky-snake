#include "MathUtils.h"
#include <math.h>

float normalizeAngle(float x) {
	x = fmod(x, 360.0f);

	if (x >= 180.0f) {
		x -= 360.0f;
	} else if (x < -180.0f) {
		x += 360.0f;
	}

	return x;
}