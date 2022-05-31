#include "genome.h"
#include "sequence.h"
#include <stdlib.h>

struct genome get_new_genome(unsigned int inputs, unsigned int outputs) {
	struct genome new_genome = {
		.connections = {
			.count = 0,
			.capacity = INITIAL_CONNECTION_CAPACITY,
			.buffer = calloc(INITIAL_CONNECTION_CAPACITY, sizeof(struct connection)),
			.realloc_amt = INITIAL_CONNECTION_CAPACITY
		},
		.input_genes = {
			.count = 0,
			.capacity = inputs,
			.buffer = calloc(inputs, sizeof(struct gene)),
			.realloc_amt = inputs
		},
		.hidden_genes = {
			.count = 0,
			.capacity = INITIAL_HIDDEN_CAPACITY,
			.buffer = calloc(INITIAL_HIDDEN_CAPACITY, sizeof(struct gene)),
			.realloc_amt = INITIAL_HIDDEN_CAPACITY
		},
		.output_genes = {
			.count = 0,
			.capacity = outputs,
			.buffer = calloc(outputs, sizeof(struct gene)),
			.realloc_amt = outputs
		}
	};

	//input genes have negative odd ids
	for (int i = 0; i < inputs; i++) {
		struct gene input_gene = {
			.id = -1 - (i * 2),
			.distance = 0
		};

		sequence_add_gene(&new_genome.input_genes, input_gene);
	}

	//output genes have negative even ids
	for (int i = 0; i < outputs; i++) {
		struct gene output_gene = {
			.id = -2 - (i * 2),
			.distance = 1
		};

		sequence_add_gene(&new_genome.output_genes, output_gene);
	}

	return new_genome;
}

void free_genome(struct genome* genome) {
	free(genome->connections.buffer);
	free(genome->input_genes.buffer);
	free(genome->output_genes.buffer);
	free(genome->hidden_genes.buffer);
}

void mutate_add_connection(struct genome* genome, struct innovation_context* context, bool allow_recurrent, unsigned int seed) {
	srand(seed);
	int inputs_count = genome->input_genes.count;
	int hidden_count = genome->hidden_genes.count;
	int output_count = genome->output_genes.count;
	int total_count = inputs_count + hidden_count + output_count;
	int possible_first_count;
	if (allow_recurrent) {
		possible_first_count = total_count;
	}
	else {
		possible_first_count = inputs_count + hidden_count;
	}

	int first_choice = rand() % (possible_first_count);
	struct gene* first_gene;
	if (first_choice < inputs_count) {
		first_gene = &genome->input_genes.buffer[first_choice];
	}
	else if (first_choice < inputs_count + hidden_count){
		first_gene = &genome->hidden_genes.buffer[first_choice - inputs_count];
	}
	else {
		first_gene = &genome->output_genes.buffer[first_choice - hidden_count - inputs_count];
	}

	int possible_second_count = 0;
	struct gene** possible_second_genes = calloc(total_count, sizeof(struct gene*));

	for (int i = 0; i < total_count; i++) {
		struct gene* gene;
		if (i < inputs_count) {
			gene = &genome->input_genes.buffer[i];
		}
		else if (i < inputs_count + hidden_count) {
			gene = &genome->hidden_genes.buffer[i - inputs_count];
		}
		else {
			gene = &genome->output_genes.buffer[i - inputs_count - hidden_count];
		}

		if (gene->distance == first_gene->distance) {
			continue;
		}
		else if (!allow_recurrent && gene->distance < first_gene->distance) {
			continue;
		}
		possible_second_genes[possible_second_count] = gene;
		possible_second_count++;
	}

	
	int second_choice = rand() % possible_second_count;
	if (possible_second_count == 0) { return; }
	struct gene* second_gene = possible_second_genes[second_choice];
	free(possible_second_genes);

	struct add_connection_innovation innovation = get_add_connection_innovation(context, first_gene->id, second_gene->id);
	struct connection new_connection = {
		.innovation_number = innovation.innovation_number,
		.first_gene = first_gene,
		.second_gene = second_gene,
		.weight = rand() / RAND_MAX,
		.enabled = true,
		.split = false
	};

	sequence_add_connection(&genome->connections, new_connection);
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
			if (connections[i].first_gene->id == origin_id && !contains_int(expanded_ids, *expanded_ids_count, origin_id)) {
				connections[i].second_gene->distance += 1;
				expanded_ids[*expanded_ids_count] = connections[i].second_gene->id;
				(*expanded_ids_count)++;
				push_genes(connections, connection_count, connections[i].second_gene->id, expanded_ids, expanded_ids_count, recurrent);
			}
		}
	}
}

void mutate_add_gene(struct genome* genome, struct innovation_context* context, unsigned int seed) {
	if (genome->connections.count == 0) { return; }
	int inputs_count = genome->input_genes.count;
	int hidden_count = genome->hidden_genes.count;
	int output_count = genome->output_genes.count;
	int total_count = inputs_count + hidden_count + output_count;
	srand(seed);
	int splittable_count = 0;
	struct connection** splittable_connections = calloc(genome->connections.count, sizeof(struct connection*));
	for (int i = 0; i < genome->connections.count; i++) {
		if (!genome->connections.buffer[i].split && genome->connections.buffer[i].enabled) {
			splittable_connections[splittable_count] = &genome->connections.buffer[i];
		}
	}
	if (splittable_count == 0) { return; }
	struct connection* connection_to_split = splittable_connections[rand() % splittable_count];
	free(splittable_connections);
	connection_to_split->enabled = false;
	connection_to_split->split = true;
	bool is_recurrent = connection_to_split->first_gene->distance > connection_to_split->second_gene->distance;
	struct add_gene_innovation innovation = get_add_gene_innovation(context, connection_to_split->innovation_number);
	struct gene new_gene = {
		.id = connection_to_split->innovation_number
	};

	if (is_recurrent) {
		if (connection_to_split->first_gene->distance - connection_to_split->second_gene->distance == 1) {
			int* expanded = calloc(total_count, sizeof(int));
			expanded[0] = connection_to_split->second_gene->id;
			int expanded_count = 1;
			push_genes(genome->connections.buffer, genome->connections.count, connection_to_split->second_gene->id, expanded, &expanded_count, is_recurrent);
			free(expanded);
		}
		new_gene.distance = connection_to_split->second_gene->distance + 1;
	}
	else {
		if (connection_to_split->second_gene->distance - connection_to_split->first_gene->distance == 1) {
			int* expanded = calloc(total_count, sizeof(int));
			expanded[0] = connection_to_split->first_gene->id;
			int expanded_count = 1;
			push_genes(genome->connections.buffer, genome->connections.count, connection_to_split->first_gene->id, expanded, &expanded_count, is_recurrent);
			free(expanded);
		}
		new_gene.distance = connection_to_split->first_gene->distance + 1;
	}

	struct gene* new_gene_addr = sequence_add_gene(&genome->hidden_genes, new_gene);

	struct connection new_connection_1 = {
		.innovation_number = innovation.innovation_number_1,
		.first_gene = connection_to_split->first_gene,
		.second_gene = new_gene_addr,
		.weight = 1.0,
		.split = false,
		.enabled = true
	};

	struct connection new_connection_2 = {
		.innovation_number = innovation.innovation_number_1,
		.first_gene = new_gene_addr,
		.second_gene = connection_to_split->second_gene,
		.weight = rand() / RAND_MAX,
		.split = false,
		.enabled = true
	};

	sequence_add_connection(&genome->connections, new_connection_1);
	sequence_add_connection(&genome->connections, new_connection_2);
}