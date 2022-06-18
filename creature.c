#include "creature.h"
#include "sequence.h"
#include "component.h"
#include <string.h>
#include <cglm/cglm.h>

void add_component(struct creature* creature, struct component component, bool is_origin);

struct creature create_creature(const char name[16], struct component origin_component) {
	struct creature new_creature = {
		.inputs = malloc(sizeof(struct component_sequence*)),
		.outputs = malloc(sizeof(struct component_sequence*))
	};

	strcpy(new_creature.name, name);
	glm_mat4_identity(new_creature.transform);

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
		if (is_origin) {
			creature->origin = addr;
		}
		break;
	}
	case OUTPUT:
	{
		struct component* addr = sequence_add_component(creature->outputs, component);
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
	creature->origin->children_count++;
}
void create_simple_creature(struct creature* creature_base) {
	struct component origin_sensor = create_food_sensor_component(0.4f, 0.0f, 0.8f);
	origin_sensor.this_creature = creature_base;
	*creature_base = create_creature("fernando", origin_sensor); //origin component is added to the correct sequence automatically

	struct component thruster = create_thruster_component(1.0f, 1.0f, 0.0f);
	thruster.this_creature = creature_base;

	struct component* thruster_addr = sequence_add_component(creature_base->outputs, thruster);

	add_component_to_origin(creature_base, thruster_addr);

}