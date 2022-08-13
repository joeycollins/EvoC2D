#ifndef GENE_H
#define GENE_H

#include "component.h"
#include <cglm/vec2.h>
#include <cglm/vec3.h>

struct gene {
	//structure
	int id;
	int distance;
	struct component* component;
	//evalution
	float value;
	int active_incoming_connections;
	int incoming_connections_executed;
};

struct gene_sequence {
	int capacity;
	int count;
	struct gene* buffer;
	int realloc_amt;
};

struct gene create_gene(int id, int distance, struct component* component);
#endif