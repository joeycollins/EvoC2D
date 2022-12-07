#include "collision.h"

bool is_collision_circle(float x1, float y1, float r1, float x2, float y2, float r2) {
	float rr = r1 + r2;
	float dx = x2 - x1;
	float dy = y2 - y1;
	return rr * rr > (dx * dx + dy * dy);
}
