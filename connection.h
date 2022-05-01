#ifndef CONNECTION_H
#define CONNECTION_H
#include <stdbool.h>

struct connection {
	int innovation_number;
	int first_gene_id;
	int second_gene_id;
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