#ifndef LINKED_NET_H_INCLUDED
#define LINKED_NET_H_INCLUDED

#include "linkedlist.h"

struct genome; struct component;

struct linked_network {
	int input_components_count;
	int output_components_count;
	struct component** input_components;
	struct component** output_components;
	struct linked_list network;
};

struct linked_network create_linked_network(struct genome* genome);

void evaluate_linked_network(struct linked_network* network, float (*activation_fn)(float));

void free_linked_network(struct linked_network* network);

#endif