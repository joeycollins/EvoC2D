#ifndef FOOD_H_INCLUDED
#define FOOD_H_INCLUDED

#include "render.h"
#include <cglm/mat4.h>
#include <stdbool.h>

struct renderer;

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
	unsigned int shader;
	struct food* food;
	struct vao_pool_rendering_info render_info;
	void (*update)(struct food_context*);
};

struct food_context create_food_context(int count, float radius, float cooldown, unsigned int shader);

void update_food_context(struct food_context* context);

#endif