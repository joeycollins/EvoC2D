#include "actualizer.h"
#include "component.h"
#include "creature.h"
#include "simulation.h"
#include "creaturecontext.h"
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

void thruster(struct component* component, float* value) {
	vec3 vec = {
		value[0], value[1], 0.0f
	};
	glm_vec3_scale(vec, main_simulation.delta_time, vec);
	glm_translate(component->this_creature->transform, vec);
}

void rotator(struct component* component, float* value) {
	float zaxis[3] = {0.0f, 0.0f, 1.0f};
	float amount = *value * main_simulation.delta_time;
	glm_rotate_at(component->this_creature->transform, component->local_position, amount, zaxis);
}

void asexual_reproduction(struct component* component, float* value) {
	float gate = 0.9f;
	component->cooldown_timer -= main_simulation.delta_time;
	if (*value > gate && component->cooldown_timer < 0) {
		struct creature* child = add_to_context(&main_simulation.main_creature_context);
		breed_creature_asex(component->this_creature, child, 5);
		component->this_creature->life_span -= 15;
		component->cooldown_timer = component->cooldown;
		//some sort of life donator
	}
}