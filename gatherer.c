#include "gatherer.h"
#include "globals.h"
#include "component.h"
#include "food.h"
#include "creature.h"
#include "simulation.h"
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

void get_closest_food_transform(struct food_context* context, mat4* this_transform, mat4 dest);

void get_gps(struct component* component, float* value) {
	value[0] = component->this_creature->transform[3][0];
	value[1] = component->this_creature->transform[3][1];
}

void get_vitality(struct component* component, float* value) {
	value[0] = component->this_creature->remaining_life_span / component->this_creature->life_span;
}

void food_sensor(struct component* component,float* position) {
	mat4 closest_transform;
	get_closest_food_transform(&main_simulation.main_food_context, &component->this_creature->transform, closest_transform);
	if (closest_transform == NULL) {
		glm_vec2_zero(position);
		return;
	}
	
	vec2 closest_position = { closest_transform[3][0] - component->this_creature->transform[3][0],
		closest_transform[3][1] - component->this_creature->transform[3][1] };

	glm_vec2_normalize(closest_position);
	glm_vec2_copy(closest_position, position);
}

float quick_magnitude_2d(mat4 transform1, mat4 transform2) {
	float delta_x = transform2[3][0] - transform1[3][0];
	float delta_y = transform2[3][1] - transform1[3][1];
	return (delta_x * delta_x) + (delta_y * delta_y);
}

void get_closest_food_transform(struct food_context* context, mat4* this_transform, mat4 dest) {
	mat4 closest;
	float dist = FLT_MAX;
	for (int i = 0; i < context->food_count; i++) {
		if (!context->food[i].alive) { continue; }
		float sqrMag = quick_magnitude_2d(this_transform, context->food[i].transform);
		if (sqrMag < dist) {
			dist = sqrMag;
			glm_mat4_copy(context->food[i].transform, closest);
		}
	}
	glm_mat4_copy(closest, dest);
}

void creature_sensor(struct component* component, float* position) {


}

void get_closest_creature_transform(struct component* component, 
	struct organism* existing_organisms, int count) {
	mat4 closest;
	float closest_mag = FLT_MAX;
	for (int i = 0; i < count; i++) {
		struct creature* creature = NULL; //TEMP NOT CORRECT
		if (creature == component->this_creature) {
			continue;
		}
		float mag = quick_magnitude_2d(component->this_creature->transform,
			creature->transform);
		if (mag < closest_mag) {
			closest_mag = mag;
			glm_mat4_copy(creature->transform, closest);
		}
	}
	return closest;
}

void get_eps(struct component* component, float* value) {
	*value = component->this_creature->life_span;
}