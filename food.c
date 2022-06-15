#include "food.h"
#include <stdlib.h>
#include <math.h>
#include <cglm/cglm.h>
#include <time.h>
#include <stdio.h>

struct food_context create_food_context(int count, float radius) {

	struct food_context context = {
		.capacity = count,
		.radius = radius,
		.food_count = 0,
		.food = malloc(sizeof(struct food) * count),
		.shape = create_food_model()
	};

	for (int i = 0; i < count; i++) {
		double random_num = rand();
		double r = (random_num / RAND_MAX) * 2 * M_PI;
		random_num = rand();
		float x = (float)(random_num / RAND_MAX * radius * cos(r));
		random_num = rand();
		float y = (float)(random_num / RAND_MAX * radius * sin(r));
		vec3 translation = { x, y , 0.0f };
		vec3 scale = { 100.1f, 100.1f, 1.0f };
		mat4 transform;
		glm_mat4_identity(transform);
		glm_translate(transform, translation);
		glm_scale(transform, scale);
		struct food new_food = {
			.alive = true,
			.energy = 100
		};

		glm_mat4_copy(transform, new_food.transform);
		context.food[context.food_count] = new_food;
		context.food_count += 1;

	}

	return context;

}
