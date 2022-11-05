#include "food.h"
#include "simulation.h"
#include "shapes.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

void position_food(struct food* food, float radius);
void render_food(struct food_context* context, struct food* food);

void update_food_context(struct food_context* context) {
	for (int i = 0; i < context->food_count; i++) {
		struct food* food = &context->food[i];
		if (!food->alive) {
			food->cooldown_timer += main_simulation.delta_time;
			if (food->cooldown_timer > context->cooldown) {
				food->cooldown_timer = 0.0f;
				food->alive = true;
				position_food(food, context->radius);
			}
		}
		else {
			render_food(context, food);
		}
	}
}

void render_food(struct food_context* context, struct food* food) {
	unsigned int shader = context->shader;
	use_shader(shader);
	unsigned int uniform_loc = glGetUniformLocation(shader, "transform");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)food->transform);
	uniform_loc = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)main_simulation.view);
	draw_from_pool_ebo(context->render_info);
}

struct food_context create_food_context(int count, float radius, float cooldown, unsigned int shader) {

	struct food_context context = {
		.capacity = count,
		.radius = radius,
		.food_count = 0,
		.food = malloc(sizeof(struct food) * count),
		.cooldown = cooldown,
		.shader = shader,
		.update = &update_food_context
	};

	struct shape food_shape = create_food_model();
	struct vao_pool_rendering_info rend_info = add_shape_to_pool(&main_simulation.vao_pool, food_shape);
	free_shape(&food_shape);

	context.render_info = rend_info;

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
	vec3 scale = { 100, 100, 1.0f };
	mat4 transform;
	get_random_translation(0, 0, radius, transform);
	glm_scale(transform, scale);
	glm_mat4_copy(transform, food->transform);
}