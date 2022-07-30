#include "food.h"
#include <stdlib.h>
#include <math.h>
#include <cglm/cglm.h>
#include <time.h>
#include <stdio.h>

extern float food_scale_factor;
extern float delta_time;

void position_food(struct food* food, float radius);

void update_food_context(struct food_context* context, struct renderer* renderer) {
	for (int i = 0; i < context->food_count; i++) {
		struct food* food = &context->food[i];
		if (!food->alive) {
			food->cooldown_timer += delta_time;
			if (food->cooldown_timer > context->cooldown) {
				food->cooldown_timer = 0.0f;
				food->alive = true;
				position_food(food, context->radius);
			}
		}
		else {
			render(&food->transform, renderer);
		}
	}
}
struct food_context create_food_context(int count, float radius, float cooldown) {

	struct food_context context = {
		.capacity = count,
		.radius = radius,
		.food_count = 0,
		.food = malloc(sizeof(struct food) * count),
		.shape = create_food_model(),
		.cooldown = cooldown
	};

	for (int i = 0; i < count; i++) {
		struct food new_food = {
			.alive = true,
			.energy = 100,
			.cooldown_timer = 0.0f
		};
		context.food[context.food_count] = new_food;
		position_food(context.food + context.food_count, radius);
		context.food_count += 1;

	}

	return context;

}

void position_food(struct food* food, float radius) {
	double random_num = rand();
	double r = (random_num / RAND_MAX) * 2 * M_PI;
	random_num = rand();
	float x = (float)(random_num / RAND_MAX * radius * cos(r));
	random_num = rand();
	float y = (float)(random_num / RAND_MAX * radius * sin(r));
	vec3 translation = { x, y , 0.0f };
	vec3 scale = { food_scale_factor, food_scale_factor, 1.0f };
	mat4 transform;
	glm_mat4_identity(transform);
	glm_translate(transform, translation);
	glm_scale(transform, scale);
	glm_mat4_copy(transform, food->transform);
}