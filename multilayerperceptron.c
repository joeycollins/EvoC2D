#include "multilayerperceptron.h"
#include "component.h"
#include <stdbool.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <stdlib.h>

int compare_ints(const void* a, const void* b);

/* Create a neural network using the provided genome. Network assumes a fully-connected network. Layers 
with missing connections use 0 as the weight in the layer matrix. For each layer matrix, the weights are ordered in
increasing source gene ID order. Hopefully we can use these matrices to increase performance using CUDA later.
A new network should only be created once a creature spawns or a generation ends (or begins). 
Pretty expensive call; so use it correctly*/
struct multilayer_perceptron create_multilayer_perceptron(struct genome* genome) {
	int network_matrices_count = 0;
	struct network_matrix* network_matrices = malloc(sizeof(struct network_matrix) * (genome->layers - 1));

	int input_vector_size = 0;
	
	for (int i = 0; i < genome->input_genes.count; i += 1) {
		switch (genome->input_genes.buffer[i].component->io_component.input.size) {
		case V2:
			input_vector_size += 2;
			break;
		case V3:
			input_vector_size += 3;
			break;
		}
	}

	int last_layer_size = input_vector_size;
	int last_layer_count = 0;
	int* last_layer_ids = malloc(input_vector_size * sizeof(int));
	if (last_layer_ids == NULL) { return; }
	for (int i = 0; i < genome->input_genes.count; i += 1) { //inputs need to be sorted in (decreasing) order in the genome sequence
		switch (genome->input_genes.buffer[i].component->io_component.input.size) {
		case V2:
			last_layer_ids[last_layer_count] = genome->input_genes.buffer[i].id;
			last_layer_ids[last_layer_count + 1] = genome->input_genes.buffer[i].id;
			last_layer_count += 2;
			break;
		case V3:
			last_layer_ids[last_layer_count] = genome->input_genes.buffer[i].id;
			last_layer_ids[last_layer_count + 1] = genome->input_genes.buffer[i].id;
			last_layer_ids[last_layer_count + 2] = genome->input_genes.buffer[i].id;
			last_layer_count += 3;
			break;
		}
	}

	for (int i = 1; i < genome->layers; i++) { //skip the input layer (layer 0)
		int layer_count = 0;
		for (int j = 0; j < genome->hidden_genes.count; j++) { //cleaner for memory in this case
			if (genome->hidden_genes.buffer[j].distance == i) {
				layer_count += 1;
			}
		}

		if (layer_count == 0) {
			continue;
		}
		
		int layer_ids_count = 0;
		int* layer_ids = malloc(sizeof(int) * layer_count);
		if (layer_ids == NULL) { return; }
		for (int j = 0; j < genome->hidden_genes.count; j++) { 
			if (genome->hidden_genes.buffer[j].distance == i) {
				layer_ids[layer_ids_count] = genome->hidden_genes.buffer[j].id;
			}
		}

		qsort(layer_ids, layer_ids_count, sizeof(int), compare_ints);
		
		struct network_matrix layer_matrix = {
			.columns = layer_ids_count,
			.rows = last_layer_count
		};

		layer_matrix.buffer = malloc(sizeof(float*) * layer_matrix.columns);

		for (int j = 0; j < layer_ids_count; j++) { //each node has a column of the matrix
			int weight_column_count = 0;
			float* weight_column = malloc(sizeof(float) * last_layer_size);
			for (int k = 0; k < last_layer_count; k++) {
				bool found_connection = false;
				for (int l = 0; l < genome->connections.count; l++) {
					if (genome->connections.buffer[l].first_gene->id == last_layer_ids[k] &&
						genome->connections.buffer[l].second_gene->id == layer_ids[j]) { //check that connection source and destination is correct
						weight_column[weight_column_count] = genome->connections.buffer[l].weight;
						found_connection = true;
						break;
					}
				}
				if (!found_connection) {
					weight_column[weight_column_count] = 0.0f;
				}
				weight_column_count++;
			}
			layer_matrix.buffer[j] = weight_column; //add the column to the matrix
		}

		network_matrices[network_matrices_count] = layer_matrix;
		network_matrices_count++;

		free(last_layer_ids);
		last_layer_ids = layer_ids;
		last_layer_count = layer_count; //layer_count and layer_ids_count are the same
	}

	free(last_layer_ids);

	struct multilayer_perceptron network = {
		.matrices = network_matrices,
		.matrices_count = network_matrices_count,
		.input_vector_size = input_vector_size
	};

	return network;
}


int compare_ints(const void* a, const void* b) {
	int* x = (int*)a;
	int* y = (int*)b;
	if (*x > *y) {
		return 1;
	}
	if (*y >= *x) {
		return -1;
	}
}

void evaluate(struct multilayer_perceptron* network) {
	return;
}

void free_multilayer_perceptron(struct multilayer_perceptron* network) {
	for (int i = 0; i < network->matrices_count; i++) {
		for (int j = 0; j < network->matrices[i].columns; j += 1) {
			free(network->matrices[i].buffer[j]);
		}
		free(network->matrices[i].buffer);
	}
	free(network->matrices);
}