#ifndef CREATURE_H_INCLUDED
#define CREATURE_H_INCLUDED

#include "component.h"
#include "shapes.h"
#include "genome.h"
#include "linkednetwork.h"
#include "render.h"
#include <cglm/mat4.h>
#include <stdbool.h>
#define INITIAL_IO_CAPACITY 10

struct creature_context;

enum life_stage {
	STAGING,
	ALIVE,
	CORPSE,
	DEAD,
	DUMP
};

struct creature {
	char name[12];
	float multiplicative_color[3]; //rgb thats added to each component color, used mainly to see predator creatures
	mat4 transform;
	int generation;
	bool predator;
	float life_span;
	float remaining_life_span;
	enum life_stage life_stage;
	int real_input_count;
	int real_output_count;
	int effective_input_count;
	int effective_output_count;
	struct component_sequence components;
	struct component* origin; //growth origin
	struct genome genome;
	struct linked_network network;
	struct creature_context* this_context;
	//functionality built into all creatures; sexual reproduction is controlled by a component
	void (*reproduce_asex)(struct creature*, float);
	void (*reproduce_sex)(struct creature*, struct creature*, float);
};

struct creature create_creature(float life_span, int component_count, mat4 translation, int generation);

//Creates a simple creature with a food sensor and a thruster
void create_simple_creature(struct creature* creature_base);

void create_simple_creature_2(struct creature* creature_base);

void create_simple_creature_3(struct creature* creature_base);

void free_creature(struct creature* creature);

void update_creature(struct creature* creature);

#endif