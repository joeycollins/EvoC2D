#ifndef FOOD_H
#define FOOD_H

#include "shapes.h"
#include <cglm/mat4.h>
#include <stdbool.h>

struct food {
	mat4 transform;
	float energy;
	bool alive;
	struct shape shape;
};

struct food_context {
	float radius;
	int capacity;
	int cooldown;
	int food_count;
	struct food* food;
};

struct food_context create_food_context(int count, float radius);

#endif