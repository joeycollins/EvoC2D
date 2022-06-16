#include "creature.h"
#include "sequence.h"
#include <string.h>

void add_component(struct creature* creature, struct component component, bool is_origin);

struct creature create_creature(const char name[16], struct component origin_component) {
	struct creature new_creature = {
		.inputs = malloc(sizeof(struct component_sequence*)),
		.outputs = malloc(sizeof(struct component_sequence*))
	};

	strcpy(new_creature.name, name);

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

void update_creature(struct creature* creature) {

}