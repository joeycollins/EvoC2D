#include "actualizer.h"
#include "component.h"
#include "creature.h"
#include "simulation.h"
#include "creaturecontext.h"
#include "utils.h"
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

void thruster(struct component* component, float* value) {
	vec3 vec = {
		value[0], value[1], 0.0f
	};
	glm_vec3_scale(vec, main_simulation.delta_time * 8, vec);
	glm_translate(component->this_creature->transform, vec);
}

void rotator(struct component* component, float* value) {
	float zaxis[3] = {0.0f, 0.0f, 1.0f};
	float amount = *value * main_simulation.delta_time;
	glm_rotate_at(component->this_creature->transform, component->local_position, amount, zaxis);
}

void asexual_reproduction(struct component* component, float* value) {
	float gate = 0.0f;
	component->cooldown_timer -= main_simulation.delta_time;
	if (component->cooldown_timer < 0 && main_simulation.main_creature_context.alive_creatures < MAX_POPULATION) {
		component->this_creature->reproduce_asex(component->this_creature, 15);
		component->this_creature->life_span -= 10;
		component->cooldown_timer = component->cooldown;
		main_simulation.main_creature_context.alive_creatures++;
		//some sort of life donator
	}
}


struct creature* get_closest_creature(struct creature_context* context, struct creature* this_creature) {
	struct creature* closest_creature = NULL;
	float closest_mag = FLT_MIN;
	for (int i = 0; i < context->creatures_count; i++) {
		struct creature* creature = context->creatures[i];
		if (creature->life_stage != ALIVE) {
			continue;
		}
		if (creature == this_creature) {
			continue;
		}
		float mag = creature->remaining_life_span;
		//float mag = quick_magnitude_2d(this_creature->transform,
		//	creature->transform);
		if (mag > closest_mag) {
			closest_mag = mag;
			closest_creature = creature;
		}
	}
	return closest_creature;
}

void sexual_reproduction(struct component* component, float* value) {
	float gate = -100.0f;
	float tdist = 30000;
	component->cooldown_timer -= main_simulation.delta_time;
	if (component->cooldown_timer < 0 && main_simulation.main_creature_context.alive_creatures < MAX_POPULATION) {
		struct creature* mate = get_closest_creature(&main_simulation.main_creature_context, component->this_creature);
		if (mate == NULL) { return; }
		float dist = mat4_distance_2d(component->this_creature->transform, mate->transform);
		if (dist < tdist) {
			component->this_creature->reproduce_sex(component->this_creature, mate, 15);
			component->this_creature->reproduce_sex(component->this_creature, mate, 15);
			component->this_creature->reproduce_sex(component->this_creature, mate, 15);
			component->cooldown_timer = component->cooldown;
			main_simulation.main_creature_context.alive_creatures++;
		}
	}
}

void do_fangs(struct component* component, float* value) {
	;
}