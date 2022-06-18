#ifndef CREATURE_H
#define CREATURE_H

#include "component.h"
#include <stdbool.h>
#include <cglm/mat4.h>

#define INITIAL_IO_CAPACITY 10

struct creature {
	char name[16];
	mat4 transform;
	struct component_sequence* inputs;
	struct component_sequence* outputs;
	struct component* origin; //growth origin
};

struct creature_context {
	int generation;
	int alive_count;
	struct creature* alive_creatures; 
};

struct creature create_creature(const char name[16], struct component origin_component);

//Creates a simple creature with a food sensor and a thruster
void create_simple_creature(struct creature* creature_base);

#endif