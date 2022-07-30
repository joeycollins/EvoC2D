#include "component.h"
#include "actualizer.h"
#include "gatherer.h"

struct component create_thruster_component(float r, float g, float b) {
	struct component nthruster = {
		.color = {r,g,b},
		.children_count = 0,
		.io_type = OUTPUT,
		.io_component = {
			.vector_size = 2,
			.activity = &thruster
		}
	};
	return nthruster;
}

struct component create_food_sensor_component(float r, float g, float b) {
	struct component sensor = {
		.color = {r,g,b},
		.children_count = 0,
		.io_type = INPUT,
		.io_component = {
			.vector_size = 2,
			.activity  = &food_sensor
		},
	};
	return sensor;
}

struct component create_rotator_component(float r, float g, float b) {
	struct component rot= {
		.color = {r,g,b},
		.children_count = 0,
		.io_type = OUTPUT,
		.io_component = {
			.vector_size = 1,
			.activity = &rotator
		}
	};
	return rot;
}

void attach_collider(struct component* component, bool isEnabled) {
	struct collider coll = {
		.enabled = isEnabled,
		.object = component,
		.layer = CREATURE_LAYER,
		.collision_fn = &is_collision_circle
	};

	component->collider = coll;
}

void get_component_position(struct component* component, vec2* dest) {
	vec2 pos = {
		component->this_creature->transform[3][0], //return creature pos for now
		component->this_creature->transform[3][1]
	};
	glm_vec2_copy(pos, dest);
}