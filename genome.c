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
	int first_choice = rand() % (hidden_count + output_count);
	struct gene first_gene;
	if (first_choice < hidden_count) {
		first_gene = genome->hidden_genes.buffer[first_choice];
	}
	else {
		first_gene = genome->output_genes.buffer[first_choice];
	}

	struct gene_sequence possible_second_genes = {
		.count = 0,
		.capacity = inputs_count + output_count + hidden_count - 1
	};
	possible_second_genes.buffer = calloc(possible_second_genes.capacity, sizeof(struct gene));

	for (int i = 0; i < inputs_count + hidden_count + output_count; i++) {
		struct gene gene;
		if (i < inputs_count) {
			gene = genome->input_genes.buffer[i];
		}
		else if (i < hidden_count) {
			gene = genome->hidden_genes.buffer[i];
		}
		else {
			gene = genome->output_genes.buffer[i];
		}

		if (gene.distance == first_gene.distance) {
			continue;
		}
		else if (!allow_recurrent && gene.distance < first_gene.distance) {
			continue;
		}

		sequence_add_gene(&possible_second_genes, gene);
	}

	if (possible_second_genes.buffer == NULL) { //no possible connection from first gene
		return;
	}
	int second_choice = rand() % possible_second_genes.count;
	struct gene second_gene = possible_second_genes.buffer[second_choice];
	free(possible_second_genes.buffer);

	struct add_connection_innovation innovation = get_add_connection_innovation(context, first_gene.id, second_gene.id);
	struct connection new_connection = {
		.innovation_number = innovation.innovation_number,
		.first_gene_id = first_gene.id,
		.second_gene_id = second_gene.id,
		.weight = rand() / RAND_MAX,
		.enabled = true,
		.split = false
	};

	sequence_add_connection(&genome->connections, new_connection);
}