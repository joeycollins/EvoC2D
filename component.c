#include "component.h"
#include "creature.h"
#include "actualizer.h"
#include "gatherer.h"
#include <string.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
	strcpy(nthruster.name, "Thruster");
	return nthruster;
}

struct component create_food_sensor_component() {
	struct component sensor = {
		.children_count = 0,
		.io_type = INPUT,
		.activity_type = FOOD_SENSOR,
		.io_component = {
			.vector_size = 3,
			.ids = malloc(sizeof(int)*3),
			.activity  = &food_sensor
		},
	};
	strcpy(sensor.name, "Food Sensor");
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
	strcpy(rot.name, "Rotator");
	return rot;
}


struct component create_asex_repro_component() {
	struct component rot = {
		.children_count = 0,
		.io_type = OUTPUT,
		.activity_type = ASEX_REPRO,
		.cooldown = 20,
		.cooldown_timer = 20,
		.io_component = {
			.vector_size = 1,
			.ids = malloc(sizeof(int) * 1),
			.activity = &asexual_reproduction
		}
	};
	strcpy(rot.name, "Asexual Reproduction");
	return rot;
}


struct component create_energy_meter_component() {
	struct component rot = {
		.children_count = 0,
		.io_type = INPUT,
		.activity_type = ENERGY_METER,
		.io_component = {
			.vector_size = 1,
			.ids = malloc(sizeof(int) * 1),
			.activity = &get_eps
		}
	};
	strcpy(rot.name, "Energy Meter");
	return rot;
}

struct component create_creature_sensor_component() {
	struct component cs = {
		.children_count = 0,
		.io_type = INPUT,
		.activity_type = CREATURE_SENSOR,
		.io_component = {
			.vector_size = 3,
			.ids = malloc(sizeof(int) * 3),
			.activity = &creature_sensor
		}
	};
	strcpy(cs.name, "Creature Sensor");
	return cs;
}

struct component create_abyss_sensor_component() {
	struct component cs = {
		.children_count = 0,
		.io_type = INPUT,
		.activity_type = ABYSS_SENSOR,
		.io_component = {
			.vector_size = 3,
			.ids = malloc(sizeof(int) * 3),
			.activity = &abyss_sensor
		}
	};
	strcpy(cs.name, "Abyss Sensor");
	return cs;
}


struct component create_sexual_reproduction_component() {
	struct component sr = {
		.children_count = 0,
		.io_type = OUTPUT,
		.activity_type = SEXUAL_REPRO,
		.cooldown = 25,
		.cooldown_timer = 25,
		.io_component = {
			.vector_size = 0,
			.ids = malloc(sizeof(int) * 0),
			.activity = &sexual_reproduction
		}
	};
	strcpy(sr.name, "Sexual Reproduction");
	return sr;
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
	case ASEX_REPRO:
		res = create_asex_repro_component();
		break;
	case ENERGY_METER:
		res = create_energy_meter_component();
		break;
	case CREATURE_SENSOR:
		res = create_creature_sensor_component();
		break;
	case SEXUAL_REPRO:
		res = create_sexual_reproduction_component();
		break;
	case ABYSS_SENSOR:
		res = create_abyss_sensor_component();
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

void free_component(struct component* component) {
	free(component->genes.buffer);
	free(component->io_component.ids);
}