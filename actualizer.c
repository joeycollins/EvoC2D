#include "actualizer.h"
#include <cglm/cglm.h>

void thruster(struct creature* creature, struct component* component, vec2 value) {
	glm_translate(creature->transform, value);
}

void rotator(struct creature* creature, struct component* component, float value) {
	float zaxis[3] = {0.0f, 0.0f, 1.0f};
	glm_rotate_at(creature->transform, component->local_position, value, zaxis);
}