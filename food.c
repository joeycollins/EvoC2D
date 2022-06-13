#include "food.h"
#include <stdlib.h>
#include <math.h>
#include <cglm/cglm.h>

struct food_context create_food_context(int count, float radius) {
	srand(0);

	struct food_context context = {
		.capacity = count,
		.radius = radius,
		.food_count = 0,
		.food = malloc(sizeof(struct food) * count)
	};

	for (int i = 0; i < count; i++) {
		double r = rand() / RAND_MAX * 2 * M_PI;
		float x = (float)(radius * cos(r));
		float y = (float)(radius * sin(r));
		vec3 translation = { x, y , 0.0f };
		mat4 transform;
		glm_mat4_identity(transform);
		glm_translate(transform, translation);
		
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
