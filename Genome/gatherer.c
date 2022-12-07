#include "gatherer.h"
#include "globals.h"
#include "component.h"
#include "food.h"
#include "creature.h"
#include "utils.h"
#include "simulation.h"
#include "abysscontext.h"
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

void get_closest_food_transform(struct food_context* context, mat4* this_transform, mat4 dest);
void get_closest_creature_transform(struct creature_context* context, struct creature* this_creature, mat4 dest);


void food_sensor(struct component* component,float* position) {
	mat4 closest_transform;
	get_closest_food_transform(&main_simulation.main_food_context, &component->this_creature->transform, closest_transform);
	if (closest_transform == NULL) {
		glm_vec2_zero(position);
		return;
	}
	float distance = mat4_distance_2d(closest_transform, component->this_creature->transform);
	distance = normalize(-1, 1, 0, MAX_SIGHT, distance);
	
	vec3 closest_position = { closest_transform[3][0] - component->this_creature->transform[3][0],
		closest_transform[3][1] - component->this_creature->transform[3][1], distance };


	glm_vec2_normalize(closest_position); //only normalize the positional components
	glm_vec3_copy(closest_position, position);
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

	float distance = mat4_distance_2d(closest_transform, component->this_creature->transform);
	distance = normalize(-1, 1, 0, MAX_SIGHT, distance);

	vec3 closest_position = { closest_transform[3][0] - component->this_creature->transform[3][0],
		closest_transform[3][1] - component->this_creature->transform[3][1], distance };

	glm_vec2_normalize(closest_position); //only normalize the positional components
	glm_vec3_copy(closest_position, position);
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
	*value = normalize(-1, 1, 0, component->this_creature->life_span * 2.5f, component->this_creature->remaining_life_span);
}

void get_closest_abyss_transform(struct abyss_context* context, struct creature* this_creature, mat4 dest);

void abyss_sensor(struct component* component, float* position) {
	mat4 closest_transform;
	get_closest_abyss_transform(&main_simulation.main_abyss_context, component->this_creature, closest_transform);
	if (closest_transform == NULL) {
		glm_vec2_zero(position);
		return;
	}

	float distance = mat4_distance_2d(closest_transform, component->this_creature->transform);
	distance = normalize(-1, 1, 0, MAX_SIGHT, distance - 200);

	vec3 closest_position = { closest_transform[3][0] - component->this_creature->transform[3][0],
		closest_transform[3][1] - component->this_creature->transform[3][1], distance };

	glm_vec2_normalize(closest_position); //only normalize the positional components
	glm_vec3_copy(closest_position, position);
}

void get_closest_abyss_transform(struct abyss_context* context, struct creature* this_creature, mat4 dest) {
	mat4 closest;
	float closest_mag = FLT_MAX;
	for (int i = 0; i < context->abysses_count; i++) {
		struct abyss* abyss = context->abysses + i;
		float mag = quick_magnitude_2d(this_creature->transform,
			abyss->transform);
		if (mag < closest_mag) {
			closest_mag = mag;
			glm_mat4_copy(abyss->transform, closest);
		}
	}
	glm_mat4_copy(closest, dest);
}