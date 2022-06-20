#include "gatherer.h"
#include "globals.h"
#include <cglm/mat4.h>
#include <cglm/cglm.h>

mat4* get_closest_food_transform(struct food_context* context, mat4* this_transform);

void food_sensor(struct component* component,vec2* position) {
	mat4* closest_transform = get_closest_food_transform(&main_food_context, &component->this_creature->transform);
	if (closest_transform == NULL) {
		return;
	}
	mat4 new_mat;
	glm_mat4_copy(*closest_transform, new_mat);
	vec2 closest_position = { new_mat[3][0], new_mat[3][1] };
	glm_vec2_copy(closest_position, position);
}

mat4* get_closest_food_transform(struct food_context* context, mat4* this_transform) {
	mat4** closest = malloc(sizeof(mat4*));
	if (closest == NULL) { return NULL; }
	float dist = FLT_MAX;
	float this_x = *this_transform[3][0];
	float this_y = *this_transform[3][1];
	for (int i = 0; i < context->food_count; i++) {
		float delta_x = context->food[i].transform[3][0] - this_x;
		float delta_y = context->food[i].transform[3][1] - this_y;
		float sqrMag = (delta_x * delta_x) + (delta_y * delta_y);
		if (sqrMag < dist) {
			dist = sqrMag;
			closest[0] = &context->food[i].transform;
		}
	}
	mat4* result;
	if (closest != NULL) {
		result = *closest;
	}
	else {
		result = NULL;
	}
	free(closest);
	return result;
}