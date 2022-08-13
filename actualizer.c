#include "actualizer.h"
#include <cglm/cglm.h>

void thruster(struct component* component, float* value) {
	vec3 vec = {
		value[0], value[1], 0.0f
	};
	glm_vec3_scale(vec, delta_time * 0.1f, vec);
	glm_translate(component->this_creature->transform, vec);
}

void rotator(struct component* component, float* value) {
	float zaxis[3] = {0.0f, 0.0f, 1.0f};
	float amount = *value * delta_time;
	glm_rotate_at(component->this_creature->transform, component->local_position, amount, zaxis);
}

