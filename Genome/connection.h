#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include <stdbool.h>

struct gene;

struct connection {
	int innovation_number;
	struct gene* first_gene;
	struct gene* second_gene;
	float weight;
	bool enabled;
	bool split;
};

struct connection_sequence {
	int capacity;
	int count;
	struct connection* buffer;
	int realloc_amt;
};

//unbounded allows for safe realloc
struct unbounded_connection_sequence {
	int capacity;
	int count;
	struct connection** buffer;
	int realloc_amt;
};

void mutate_connection(struct connection* connection, float mutate_weight_chance, float mutate_enabled_chance);

#endif