#include "linkednetwork.h"
#include <stdlib.h>


struct linked_network create_linked_network(struct genome* genome) {
	struct linked_list connections = new_list();


	for (int i = 0; i < genome->layers; i++) {
		for (int j = 0; j < genome->connections.count; j++) {
			if (genome->connections.buffer[j].enabled && genome->connections.buffer[j].first_gene->distance == i) {
				list_add(&connections, genome->connections.buffer + j);
			}
		}
	}

	struct linked_network new_network = {
		.network = connections,
		.input_groups_count = genome->input_groups_count,
		.output_groups_count = genome->output_groups_count,
		.input_groups = genome->input_groups,
		.output_groups = genome->output_groups
	};

	return new_network;
}

void evaluate_linked_network(struct linked_network* network, float (*activation_fn)(float)) {
	//set the inputs for this frame
	for (int i = 0; i < network->input_groups_count; i++) {
		struct linked_list* group = network->input_groups + i;
		struct component* this_component = ((struct gene*)group->head->data)->component;
		float* buffer = malloc(this_component->io_component.vector_size * sizeof(float));
		this_component->io_component.activity(this_component, buffer);
		struct link* current_in = group->head;
		for (int j = 0; j < this_component->io_component.vector_size; j++) {
			struct gene* g = ((struct gene*)current_in->data);
			((struct gene*)current_in->data)->value = buffer[j];
			current_in = current_in->next;
		}
		free(buffer);
	}
	
	struct link* current_node = network->network.head;
	//evaluate the connections
	for (int i = 0; i < network->network.count; i++) {
		struct connection* conn = (struct connection*)current_node->data;
		if (conn->second_gene->incoming_connections_executed == 0) {
			conn->second_gene->value = activation_fn(conn->first_gene->value * conn->weight);
		}
		else {
			conn->second_gene->value += activation_fn(conn->first_gene->value * conn->weight);
		}
		conn->second_gene->incoming_connections_executed++;
		if (conn->second_gene->incoming_connections_executed == conn->second_gene->active_incoming_connections) {
			conn->second_gene->incoming_connections_executed = 0; // reset the node, no more connections on this iteration
		}
		current_node = current_node->next;
	}

	//actualize the results
	for (int i = 0; i < network->output_groups_count; i++) {
		struct linked_list* group = network->output_groups + i;
		struct component* this_component = ((struct gene*)group->head->data)->component;
		float* buffer = malloc(this_component->io_component.vector_size * sizeof(float));
		struct link* current = group->head;
		for (int j = 0; j < group->count; j++) {
			buffer[j] = ((struct gene*)current->data)->value;
			current = current->next;
		}
		this_component->io_component.activity(this_component, buffer);
		free(buffer);
	}
}

void free_linked_network(struct linked_network* network) {
	free_list(&network->network);
}