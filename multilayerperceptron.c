#include "multilayerperceptron.h"
#include "component.h"
#include <stdbool.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <stdlib.h>
#include <cblas/cblas.h>

//aligned malloc used for improved BLAS performance

//int compare_ints(const void* a, const void* b);

/* Create a neural network using the provided genome. Network assumes a fully-connected network. Layers 
with missing connections use 0 as the weight in the layer matrix. For each layer matrix, the weights are ordered in
increasing source gene ID order. Hopefully we can use these matrices to increase performance using CUDA later.
A new network should only be created once a creature spawns or a generation begins. Pretty expensive call; so use 
it correctly*/
/*
struct multilayer_perceptron create_multilayer_perceptron(struct genome* genome) {
	int network_matrices_count = 0;
	struct network_matrix* network_matrices = malloc(sizeof(struct network_matrix) * (genome->layers - 1));
	struct component** input_components = malloc(sizeof(struct component*) * genome->input_genes.count);
	struct component** output_components = malloc(sizeof(struct component*) * genome->output_genes.count);

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
		input_components[i] = genome->input_genes.buffer[i].component;
	}

	int output_vector_size = 0;
	for (int i = 0; i < genome->output_genes.count; i += 1) {
		switch (genome->output_genes.buffer[i].component->io_component.output.size) {
		case V2:
			output_vector_size += 2;
			break;
		case V3:
			output_vector_size += 3;
			break;
		}
		output_components[i] = genome->output_genes.buffer[i].component;
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
		if (layer_ids == NULL) { continue; } //this wont happen
		for (int j = 0; j < genome->hidden_genes.count; j++) { 
			if (genome->hidden_genes.buffer[j].distance == i) {
				layer_ids[layer_ids_count] = genome->hidden_genes.buffer[j].id;
				layer_ids_count++;
			}
		}

		qsort(layer_ids, layer_ids_count, sizeof(int), compare_ints);
		
		struct network_matrix layer_matrix = {
			.columns = layer_ids_count,
			.rows = last_layer_count
		};

		int layer_matrix_count = 0;
		float* matrix = _aligned_malloc(sizeof(float*) * layer_matrix.columns * layer_matrix.rows, 64);
		
		/* column aligned
		for (int j = 0; j < layer_ids_count; j++) { //each node has a column of the matrix
			for (int k = 0; k < last_layer_count; k++) {
				bool found_connection = false;
				for (int l = 0; l < genome->connections.count; l++) {
					if (genome->connections.buffer[l].first_gene->id == last_layer_ids[k] &&
						genome->connections.buffer[l].second_gene->id == layer_ids[j]) { //check that connection source and destination is correct
						matrix[layer_matrix_count] = genome->connections.buffer[l].enabled ? genome->connections.buffer[l].weight : 0.0f;
						found_connection = true;
						break;
					}
				}
				if (!found_connection) {
					matrix[layer_matrix_count] = 0.0f;
				}
				layer_matrix_count++;
			}
		}*/
/*
		//row aligned for blas sgemm
		for (int j = 0; j < last_layer_count; j++) {
			for (int k = 0; k < layer_ids_count; k++) {
				bool found_connection = false;
				for (int l = 0; l < genome->connections.count; l++) {
					if (genome->connections.buffer[l].first_gene->id = last_layer_ids[j] &&
						genome->connections.buffer[l].second_gene->id == layer_ids[k]) {
						matrix[layer_matrix_count] = genome->connections.buffer[l].enabled ? genome->connections.buffer[l].weight : 0.0f;
						found_connection = true;
						break;
					}
				}
				if (!found_connection) { // there is no connection to layer_ids[k] from last_layer_ids[j]
					matrix[layer_matrix_count] = 0.0f;
				}
				layer_matrix_count++;
			}
		}
		layer_matrix.buffer = matrix; //set the pointer
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
		.input_vector_size = input_vector_size,
		.activation = &tanh,
		.output_vector_size = output_vector_size,
		.input_components = input_components,
		.output_components = output_components,
		.input_components_count = genome->input_genes.count,
		.output_components_count = genome->output_genes.count
	};

	return network;
}

//compare for qsort
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
	const float alpha = 1.0f, beta = 0.0f;
	int last_columns = 0;
	int last_rows = 1;
	float* input_vector = _aligned_malloc(network->input_vector_size * sizeof(float), 64);
	
	for (int i = 0; i < network->input_components_count; i++) {
		switch (network->input_components[i]->io_component.input.size) {
		case V2:
		{
			vec2 result;
			network->input_components[i]->io_component.input.gatherer.gather_v2(network->input_components[i], &result);
			input_vector[last_columns] = result[0];
			input_vector[last_columns+1] = result[1];
			last_columns += 2;
			break;
		}
		case V3:
		{
			vec3 result;
			network->input_components[i]->io_component.input.gatherer.gather_v3(network->input_components[i], &result);
			input_vector[last_columns] = result[0];
			input_vector[last_columns + 1] = result[1];
			input_vector[last_columns + 2] = result[2];
			last_columns += 3;
			break;
		}
		}
	}
	float** last_vector = &input_vector;

	for (int i = 0; i < network->matrices_count; i++) {
		float* result = _aligned_malloc(last_columns * network->matrices[i].rows *  sizeof(float), 64);
		//memset(result, 0.0f, last_columns * network->matrices[i].rows);
		/*cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, last_rows, network->matrices[i].columns,
			last_columns, alpha, *last_vector,network->matrices[i].rows, network->matrices[i].buffer,
			network->matrices[i].columns, beta, result, network->matrices[i].columns);*/
/*
		_aligned_free(*last_vector);

		for (int j = 0; j < network->matrices[i].columns; j++) { //apply activation
			*(result + j) = network->activation(*(result+j));
		}

		last_vector = &result;
		last_columns = network->matrices[i].columns;		
	}
	
	int actualizer_idx = 0;
	//do outputs
	for (int i = 0; i < network->output_components_count; i++) {
		switch (network->output_components[i]->io_component.output.size) {
		case V2:
		{
			vec2 result = { (*last_vector)[actualizer_idx], (* last_vector)[actualizer_idx + 1]};
			network->output_components[i]->io_component.output.actualizer.actualize_v2(network->output_components[i], &result);
			actualizer_idx += 2;
			break;
		}
		case V3:
		{
			vec3 result = { *last_vector[actualizer_idx], *last_vector[actualizer_idx + 1], *last_vector[actualizer_idx + 2] };
			network->output_components[i]->io_component.output.actualizer.actualize_v2(network->output_components[i], &result);
			actualizer_idx += 3;
			break;
		}
		}
	}

	_aligned_free(*last_vector);
}

void free_multilayer_perceptron(struct multilayer_perceptron* network) {
	for (int i = 0; i < network->matrices_count; i++) {
		free(network->matrices[i].buffer);
	}
	free(network->matrices);
	free(network->input_components); //dont free the component pointers
	free(network->output_components);
}
*/