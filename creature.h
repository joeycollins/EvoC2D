#ifndef CREATURE_H
#define CREATURE_H

#include "component.h"

#define MAX_INPUTS 16
#define MAX_OUTPUTS 16

struct creature {
	char name[16];
	int input2_count;
	struct input_component_v2 inputs2[MAX_INPUTS];
	int input3_count;
	struct input_component_v3 inputs3[MAX_INPUTS];
	int output2_count;
	struct output_component_v2 outputs2[MAX_OUTPUTS];
	int output3_count;
	struct output_component_v3 outputs3[MAX_OUTPUTS];
};

struct creature_context {
	int generation;
	int alive_count;
	struct creature* alive_creatures; 
};

void update_creature(struct creature* creature);

#endif