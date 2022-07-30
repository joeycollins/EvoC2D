#include "genome.h"
#include "sequence.h"
#include <stdlib.h>

struct genome create_genome(struct creature* creature) {
	int inputs_count = creature->inputs->count, outputs_count = creature->outputs->count;
	//these vals should be saved by creature eventually
	int input_genes_count = 0;
	for (int i = 0; i < creature->inputs->count; i++) {
		input_genes_count += creature->inputs->buffer[i].io_component.vector_size;
	}
	int output_genes_count = 0;
	for (int i = 0; i < creature->outputs->count; i++) {
		output_genes_count += creature->outputs->buffer[i].io_component.vector_size;
	}
	struct genome new_genome = {
		.connections = {
			.count = 0,
			.capacity = INITIAL_CONNECTION_CAPACITY,
			.buffer = calloc(INITIAL_CONNECTION_CAPACITY, sizeof(struct connection)),
			.realloc_amt = INITIAL_CONNECTION_CAPACITY
		},
		.input_genes = {
			.count = 0,
			.capacity = input_genes_count,
			.buffer = calloc(input_genes_count, sizeof(struct gene)),
			.realloc_amt = input_genes_count
		},
		.hidden_genes = {
			.count = 0,
			.capacity = INITIAL_HIDDEN_CAPACITY,
			.buffer = calloc(INITIAL_HIDDEN_CAPACITY, sizeof(struct gene)),
			.realloc_amt = INITIAL_HIDDEN_CAPACITY
		},
		.output_genes = {
			.count = 0,
			.capacity = output_genes_count,
			.buffer = calloc(output_genes_count, sizeof(struct gene)),
			.realloc_amt = output_genes_count
		},
		.input_groups_count = creature->inputs->count,
		.output_groups_count = creature->outputs->count,
		.input_groups = malloc(inputs_count * sizeof(struct linked_list)),
		.output_groups = malloc(outputs_count * sizeof(struct linked_list)),
		.layers = 1
	};
	//input genes have negative odd ids
	int id_incrementor = 0;
	for (int i = 0; i < creature->inputs->count; i++) {
		int comp_size = creature->inputs->buffer[i].io_component.vector_size;
		struct linked_list group = new_list();
		for (int j = 0; j < comp_size; j++) {
			struct gene input_gene = create_gene(-1 - (id_incrementor * 2), 0, &creature->inputs->buffer[i]);
			struct gene* addr = sequence_add_gene(&new_genome.input_genes, input_gene);
			list_add(&group, addr);
			id_incrementor++;
		}
		new_genome.input_groups[i] = group;
	}

	//output genes have negative even ids
	id_incrementor = 0;
	for (int i = 0; i < creature->outputs->count; i++) {
		int comp_size = creature->outputs->buffer[i].io_component.vector_size;
		struct linked_list group = new_list();
		for (int j = 0; j < comp_size; j++) {
			struct gene output_gene = create_gene(-2 - (id_incrementor * 2), 1, &creature->outputs->buffer[i]);
			struct gene* addr = sequence_add_gene(&new_genome.output_genes, output_gene);
			list_add(&group, addr);
			id_incrementor++;
		}
		new_genome.output_groups[i] = group;
	}

	return new_genome;
}

void free_genome(struct genome* genome) {
	free_list(genome->input_groups);
	free_list(genome->output_groups);
	free(genome->connections.buffer);
	free(genome->input_genes.buffer);
	free(genome->output_genes.buffer);
	free(genome->hidden_genes.buffer);
}

void mutate_add_connection(struct genome* genome, struct innovation_context* context) {
	int inputs_count = genome->input_genes.count;
	int hidden_count = genome->hidden_genes.count;
	int output_count = genome->output_genes.count;
	int possible_first_count = inputs_count + hidden_count;
	if (possible_first_count == 0) {
		return;
	}
	int random = rand();
	int first_choice = random % (possible_first_count);
	struct gene* first_gene;
	if (first_choice < inputs_count) {
		first_gene = &genome->input_genes.buffer[first_choice];
	}
	else {
		first_gene = &genome->hidden_genes.buffer[first_choice - inputs_count];
	}

	int existing_connections_count = 0;
	int* existing_connections = malloc(genome->connections.count * sizeof(int));
	for (int i = 0; i < genome->connections.count; i++) {
		if (genome->connections.buffer[i].first_gene == first_gene) {
			existing_connections[existing_connections_count] = genome->connections.buffer[i].second_gene->id;
			existing_connections_count++;
		}
	}

	int possible_second_count = 0;
	int max_possible_second_count = hidden_count + output_count;
	struct gene** possible_second_genes = malloc(max_possible_second_count * sizeof(struct gene*));
	if (possible_second_genes == NULL) { return;}
	for (int i = 0; i < max_possible_second_count; i++) {
		struct gene* gene;
		if (i < hidden_count) {
			gene = &genome->hidden_genes.buffer[i];
		}
		else {
			gene = &genome->output_genes.buffer[i - hidden_count];
		}
		if (gene->distance <= first_gene->distance) {
			continue;
		}
		bool exists = false;
		for (int j = 0; j < existing_connections_count; j++) {
			if (gene->id == existing_connections[j]) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			possible_second_genes[possible_second_count] = gene;
			possible_second_count++;
		}
	}
	free(existing_connections);

	if (possible_second_count == 0) { 
		free(possible_second_genes);
		return; 
	}
	random = rand();
	int second_choice = random % possible_second_count;
	struct gene* second_gene = possible_second_genes[second_choice];
	free(possible_second_genes);

	random = rand();
	float connections_weight = ((float)random / RAND_MAX);
	struct add_connection_innovation innovation = get_add_connection_innovation(context, first_gene->id, second_gene->id);
	struct connection new_connection = {
		.innovation_number = innovation.innovation_number,
		.first_gene = first_gene,
		.second_gene = second_gene,
		.weight = connections_weight,
		.enabled = true,
		.split = false
	};
	sequence_add_connection(&genome->connections, new_connection);

	//update gene info
	second_gene->active_incoming_connections++;
}

bool contains_int(int* sequence, int count, int element) {
	for (int i = 0; i < count; i++) {
		if (sequence[i] == element) {
			return true;
		}
	}
	return false;
}

//push genes connected to the origin +1 distance from the input layer
void push_genes(struct connection* connections, int connection_count, int origin_id, int* expanded_ids, int* expanded_ids_count, bool recurrent) {
	if (recurrent) {
		for (int i = 0; i < connection_count; i++) {
			if (connections[i].second_gene->id == origin_id && !contains_int(expanded_ids, *expanded_ids_count, origin_id)) {
				connections[i].first_gene->distance += 1;
				expanded_ids[*expanded_ids_count] = connections[i].first_gene->id;
				(*expanded_ids_count)++;
				push_genes(connections, connection_count, connections[i].first_gene->id, expanded_ids, expanded_ids_count, recurrent);
			}
		}
	}
	else {
		for (int i = 0; i < connection_count; i++) {
			if (connections[i].first_gene->id == origin_id && !contains_int(expanded_ids, *expanded_ids_count, connections[i].second_gene->id)) {
				connections[i].second_gene->distance += 1;
				expanded_ids[*expanded_ids_count] = connections[i].second_gene->id;
				(*expanded_ids_count)++;
				push_genes(connections, connection_count, connections[i].second_gene->id, expanded_ids, expanded_ids_count, recurrent);
			}
		}
	}
}

void mutate_add_gene(struct genome* genome, struct innovation_context* context) {
	if (genome->connections.count == 0) { return; }
	int inputs_count = genome->input_genes.count;
	int hidden_count = genome->hidden_genes.count;
	int output_count = genome->output_genes.count;
	int total_count = inputs_count + hidden_count + output_count;

	int splittable_count = 0;
	struct connection** splittable_connections = calloc(genome->connections.count, sizeof(struct connection*));
	if (splittable_connections == NULL) { return; }
	for (int i = 0; i < genome->connections.count; i++) {
		if (genome->connections.buffer[i].split == false && genome->connections.buffer[i].enabled) {
			splittable_connections[splittable_count] = &genome->connections.buffer[i];
			splittable_count++;
		}
	}
	if (splittable_count == 0) { 
		free(splittable_connections);
		return;
	}

	int random = rand();
	struct connection* connection_to_split = splittable_connections[random % splittable_count];
	free(splittable_connections);
	connection_to_split->enabled = false;
	connection_to_split->split = true;
	struct add_gene_innovation innovation = get_add_gene_innovation(context, connection_to_split->innovation_number);
	struct gene new_gene = create_gene(connection_to_split->innovation_number, 0, NULL);

	if (connection_to_split->second_gene->distance - connection_to_split->first_gene->distance == 1) {
		int* expanded = calloc(total_count, sizeof(int));
		expanded[0] = connection_to_split->first_gene->id;
		int expanded_count = 1;
		push_genes(genome->connections.buffer, genome->connections.count, connection_to_split->first_gene->id, expanded, &expanded_count, false);
		free(expanded);
		genome->layers++; //this could be wrong; havent tested, 
	}
	new_gene.distance = connection_to_split->first_gene->distance + 1;
	
	struct gene* new_gene_addr = sequence_add_gene(&genome->hidden_genes, new_gene);

	struct connection new_connection_1 = {
		.innovation_number = innovation.innovation_number_1,
		.first_gene = connection_to_split->first_gene,
		.second_gene = new_gene_addr,
		.weight = 1.0f,
		.split = false,
		.enabled = true
	};

	random = rand();
	struct connection new_connection_2 = {
		.innovation_number = innovation.innovation_number_1,
		.first_gene = new_gene_addr,
		.second_gene = connection_to_split->second_gene,
		.weight = (float)random / RAND_MAX,
		.split = false,
		.enabled = true
	};

	sequence_add_connection(&genome->connections, new_connection_1);
	sequence_add_connection(&genome->connections, new_connection_2);

	new_gene_addr->active_incoming_connections++;
}
