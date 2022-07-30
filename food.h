#ifndef FOOD_H
#define FOOD_H
#include "render.h"
#include "shapes.h"
#include <cglm/mat4.h>
#include <stdbool.h>

struct food {
	mat4 transform;
	float energy;
	bool alive;
	float cooldown_timer;
};

struct food_context {
	float radius;
	int capacity;
	int cooldown;
	int food_count;
	struct shape shape;
	struct food* food;
};

struct food_context create_food_context(int count, float radius, float cooldown);

void update_food_context(struct food_context* context, struct renderer* renderer);

#endif