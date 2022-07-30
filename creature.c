#include "creature.h"
#include "sequence.h"
#include "component.h"
#include <string.h>
#include <cglm/cglm.h>

//This needs alot of work 

extern float creature_scale_factor;

void add_component(struct creature* creature, struct component component, bool is_origin);

struct creature create_creature(const char name[16], struct component origin_component) {
	struct creature new_creature = {
		.inputs = malloc(sizeof(struct component_sequence*)),
		.outputs = malloc(sizeof(struct component_sequence*))
	};

	strcpy(new_creature.name, name);
	glm_mat4_identity(new_creature.transform);
	vec3 scale = { creature_scale_factor, creature_scale_factor, 1.0f };
	glm_scale(new_creature.transform, scale);

	struct component_sequence in_seq = {
			.capacity = INITIAL_IO_CAPACITY,
			.count = 0,
			.buffer = malloc(sizeof(struct component) * INITIAL_IO_CAPACITY),
			.realloc_amt = INITIAL_IO_CAPACITY
	};

	new_creature.inputs[0] = in_seq;

	struct component_sequence out_seq = {
			.capacity = INITIAL_IO_CAPACITY,
			.count = 0,
			.buffer = malloc(sizeof(struct component) * INITIAL_IO_CAPACITY),
			.realloc_amt = INITIAL_IO_CAPACITY
	};

	new_creature.outputs[0] = out_seq;

	add_component(&new_creature, origin_component, true);

	return new_creature;
}

void add_component(struct creature* creature, struct component component, bool is_origin) {
	switch (component.io_type) {
	case INPUT:
	{
		struct component* addr = sequence_add_component(creature->inputs, component);
		attach_collider(addr, true);
		if (is_origin) {
			creature->origin = addr;
		}
		break;
	}
	case OUTPUT:
	{
		struct component* addr = sequence_add_component(creature->outputs, component);
		attach_collider(addr, false);
		if (is_origin) {
			creature->origin = addr;
		}
		break;
	}
	}
}

void add_component_to_origin(struct creature* creature, struct component* component) {
	if (creature->origin->children_count >= MAX_CHILDREN) {
		return;
	}
	*(creature->origin->children+creature->origin->children_count) = component;
	attach_collider(creature->origin->children + creature->origin->children_count, false);
	creature->origin->children_count++;
}

void create_simple_creature(struct creature* creature_base) {
	float r = rand(), g = rand(), b = rand();
	r /= RAND_MAX;
	g /= RAND_MAX;
	b /= RAND_MAX;
	struct component origin_sensor = create_food_sensor_component(r, g, b);
	origin_sensor.this_creature = creature_base;
	(*creature_base) = create_creature("fernando", origin_sensor); //origin component is added to the correct sequence automatically
	r = rand(), g = rand(), b = rand();
	r /= RAND_MAX;
	g /= RAND_MAX;
	b /= RAND_MAX;
	struct component thruster = create_thruster_component(r, g, b);
	thruster.this_creature = creature_base;

	struct component* thruster_addr = sequence_add_component(creature_base->outputs, thruster);
	add_component_to_origin(creature_base, thruster_addr);

	r = rand(), g = rand(), b = rand();
	r /= RAND_MAX;
	g /= RAND_MAX;
	b /= RAND_MAX;
	struct component rot = create_rotator_component(r, g, b);
	rot.this_creature = creature_base;
	struct component* rot_addr = sequence_add_component(creature_base->outputs, rot);
	add_component_to_origin(creature_base, rot_addr);
}

void free_creature(struct creature* creature) {
	free(creature->inputs->buffer);
	free(creature->outputs->buffer);
	//free(creature->inputs);
	//free(creature->outputs);
}