#ifndef SHAPES_H
#define SHAPES_H

#include "creature.h"

struct shape {
	float* vertices;
	int vertices_count;
	unsigned int* indices;
	int indices_count;
};

struct shape create_creature_model(struct creature* creature);

void free_shape(struct shape* shape);

#endif