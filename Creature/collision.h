#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

#include <stdbool.h>

#define CREATURE_LAYER 1
#define FOOD_LAYER 2

struct collider {
	bool enabled;
	int layer;
	void* object;
	bool (*collision_fn)(float, float, float, float, float, float);
};

bool is_collision_circle(float x1, float y1, float r1, float x2, float y2, float r2);

#endif
