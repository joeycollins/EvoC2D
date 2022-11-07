#include "gatherer.h"
#include "globals.h"
#include "component.h"
#include "food.h"
#include "creature.h"
#include "utils.h"
#include "simulation.h"
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

void get_closest_food_transform(struct food_context* context, mat4* this_transform, mat4 dest);
void get_closest_creature_transform(struct creature_context* context, struct creature* this_creature, mat4 dest);

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
	mat4 closest_transform;
	get_closest_creature_transform(&main_simulation.main_creature_context, component->this_creature, closest_transform);
	if (closest_transform == NULL) {
		glm_vec2_zero(position);
		return;
	}

	vec2 closest_position = { closest_transform[3][0] - component->this_creature->transform[3][0],
		closest_transform[3][1] - component->this_creature->transform[3][1] };

	glm_vec2_normalize(closest_position);
	glm_vec2_copy(closest_position, position);
}

void get_closest_creature_transform(struct creature_context* context,struct creature* this_creature,  mat4 dest) {
	mat4 closest;
	float closest_mag = FLT_MAX;
	for (int i = 0; i < context->creatures_count; i++) {
		struct creature* creature = context->creatures[i];
		if (creature->life_stage != ALIVE) {
			continue;
		}
		if (creature == this_creature) {
			continue;
		}
		float mag = quick_magnitude_2d(this_creature->transform,
			creature->transform);
		if (mag < closest_mag) {
			closest_mag = mag;
			glm_mat4_copy(creature->transform, closest);
		}
	}
	glm_mat4_copy(closest, dest);
}

void get_eps(struct component* component, float* value) {
	*value = component->this_creature->life_span;
}