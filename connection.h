#ifndef CONNECTION_H
#define CONNECTION_H
#include "gene.h"
#include <stdbool.h>

struct connection {
	int innovation_number;
	struct gene* first_gene;
	struct gene* second_gene;
	float weight;
	bool enabled;
	bool split;
};

struct connection_sequence {
	unsigned int capacity;
	unsigned int count;
	struct connection* buffer;
	unsigned int realloc_amt;
};

#endif