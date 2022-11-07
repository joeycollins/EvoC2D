#include "linkednetwork.h"
#include "genome.h"
#include "component.h"
#include "creature.h"
#include <stdlib.h>


struct linked_network create_linked_network(struct genome* genome) {
	struct linked_network new_network = {
		.input_components_count = 0,
		.output_components_count = 0,
		.input_components = malloc(sizeof(struct component*) * genome->this_creature->real_input_count),
		.output_components = malloc(sizeof(struct component*) * genome->this_creature->real_output_count)
	};
	
	struct linked_list connections = new_list();


	for (int i = 0; i < genome->layers; i++) {
		for (int j = 0; j < genome->connections.count; j++) {
			if (genome->connections.buffer[j].enabled && genome->connections.buffer[j].first_gene->distance == i) {
				genome->connections.buffer[j].second_gene->active_incoming_connections++;
				list_add(&connections, genome->connections.buffer + j);
			}
		}
	}

	new_network.network = connections;

	for (int i = 0; i < genome->this_creature->components.count; i++) {
		struct component* this_component = genome->this_creature->components.buffer + i;
		if (this_component->io_type == INPUT) {
			new_network.input_components[new_network.input_components_count] = this_component;
			new_network.input_components_count++;
		}
		else if (this_component->io_type == OUTPUT) {
			new_network.output_components[new_network.output_components_count] = this_component;
			new_network.output_components_count++;
		}
	}

	return new_network;
}

void evaluate_linked_network(struct linked_network* network, float (*activation_fn)(float)) {
	//set the inputs for this frame
	for (int i = 0; i < network->input_components_count; i++) {
		struct component* this_component = network->input_components[i];
		float* buffer = malloc(this_component->io_component.vector_size * sizeof(float));
		this_component->io_component.activity(this_component, buffer);
		for (int j = 0; j < this_component->io_component.vector_size; j++) {
			this_component->genes.buffer[j].value = buffer[j];
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
	for (int i = 0; i < network->output_components_count; i++) {
		struct component* this_component = network->output_components[i];
		float* buffer = malloc(this_component->io_component.vector_size * sizeof(float));
		for (int j = 0; j < this_component->io_component.vector_size; j++) {
			buffer[j] = this_component->genes.buffer[j].value;
		}
		this_component->io_component.activity(this_component, buffer);
		free(buffer);
	}
}

void free_linked_network(struct linked_network* network) {
	free(network->input_components);
	free(network->output_components);
	free_list(&network->network);
}