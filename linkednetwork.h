#ifndef LINKED_NET_H
#define LINKED_NET_H

#include "linkedlist.h"
#include "genome.h"


struct linked_network {
	int input_groups_count;
	int output_groups_count;
	struct linked_list* input_groups;
	struct linked_list* output_groups;
	struct linked_list network;
};

struct linked_network create_linked_network(struct genome* genome);

void evaluate_linked_network(struct linked_network* network, float (*activation_fn)(float));

void free_linked_network(struct linked_network* network);

#endif