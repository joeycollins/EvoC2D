#include "actualizer.h"
#include <cglm/cglm.h>

void thruster(struct component* component, vec2 value) {
	glm_translate2d(component->this_creature->transform, value);
}

void rotator(struct component* component, float value) {
	float zaxis[3] = {0.0f, 0.0f, 1.0f};
	glm_rotate_at(component->this_creature->transform, component->local_position, value, zaxis);
}