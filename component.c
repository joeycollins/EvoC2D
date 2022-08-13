#include "component.h"
#include "actualizer.h"
#include "gatherer.h"
#include <cglm/cglm.h>

void set_child_exists_array(bool *dest) {
	for (int i = 0; i < MAX_CHILDREN; i++) {
		dest[i] = false;
	}
}

struct component create_thruster_component() {
	struct component nthruster = {
		.children_count = 0,
		.io_type = OUTPUT,
		.activity_type = THRUSTER,
		.io_component = {
			.vector_size = 2,
			.ids = malloc(sizeof(int) * 2),
			.activity = &thruster
		}
	};
	return nthruster;
}

struct component create_food_sensor_component() {
	struct component sensor = {
		.children_count = 0,
		.io_type = INPUT,
		.activity_type = FOOD_SENSOR,
		.io_component = {
			.vector_size = 2,
			.ids = malloc(sizeof(int)*2),
			.activity  = &food_sensor
		},
	};
	return sensor;
}

struct component create_rotator_component() {
	struct component rot= {
		.children_count = 0,
		.io_type = OUTPUT,
		.activity_type = ROTATOR,
		.io_component = {
			.vector_size = 1,
			.ids = malloc(sizeof(int) * 1),
			.activity = &rotator
		}
	};
	return rot;
}

struct component create_gps_component() {
	struct component gps = {
		.children_count = 0,
		.io_type = INPUT,
		.activity_type = GPS,
		.io_component = {
			.vector_size = 2,
			.ids = malloc(sizeof(int) * 2),
			.activity = &get_gps
		}
	};
	return gps;
}

struct component create_component(enum activity_type activity_type) {
	struct component res;
	switch (activity_type) {
	case THRUSTER:
		res = create_thruster_component();
		break;
	case ROTATOR:
		res= create_rotator_component();
		break;
	case FOOD_SENSOR:
		res = create_food_sensor_component();
		break;
	case GPS:
		res = create_gps_component();
		break;
	default:
		res = create_thruster_component();
		break;
	};
	
	set_child_exists_array(res.child_exists);
	return res;
}

void assign_color(struct component* component, float color[3]) {
	component->color[0] = color[0];
	component->color[1] = color[1];
	component->color[2] = color[2];
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
	mat4 res;
	glm_mul(component->this_creature->transform, component->local_transform, res);
	vec2 pos = {
		res[3][0],
		res[3][1]
	};

	glm_vec2_copy(pos, dest);
}
