#ifndef GENE_H
#define GENE_H

#include "component.h"
#include <cglm/vec2.h>
#include <cglm/vec3.h>

struct gene {
	int id;
	int distance;
	struct component* component;
};

struct gene_sequence {
	unsigned int capacity;
	unsigned int count;
	struct gene* buffer;
	unsigned int realloc_amt;
};
#endif