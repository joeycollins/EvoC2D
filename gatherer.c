#include "gatherer.h"
#include <cglm/mat4.h>

mat4* get_closest_food_transform(struct food_context* context, mat4* this_transform);

void food_sensor(vec2* position, struct creature* creature, struct food_context* context) {
	mat4* closest_transform = get_closest_food_transform(context, &creature->transform);
	if (closest_transform == NULL) {
		return;
	}
	vec2 closest_position = { *closest_transform[3][0], *closest_transform[3][1] };
	*position[0] = closest_position[0];
	*position[1] = closest_position[1];
}

mat4* get_closest_food_transform(struct food_context* context, mat4* this_transform) {
	mat4** closest = malloc(sizeof(mat4*));
	float dist = FLT_MAX;
	float this_x = *this_transform[3][0];
	float this_y = *this_transform[3][1];
	for (int i = 0; i < context->food_count; i++) {
		float delta_x = context->food[i].transform[3][0] - this_x;
		float delta_y = context->food[i].transform[3][1] - this_y;
		float sqrMag = (delta_x * delta_x) + (delta_y * delta_y);
		if (sqrMag < dist) {
			dist = sqrMag;
			closest = &context->food[i];
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