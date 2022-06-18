#ifndef MLAYER_H
#define MLAYER_H

#include "genome.h"

struct network_matrix {
	float** buffer;
	int rows;
	int columns;
};

struct multilayer_perceptron {
	int input_vector_size;
	int output_vector_size;
	int matrices_count;
	struct network_matrix* matrices;
	float (*activation)(float);
	float (*aggregation)(float);
};

struct multilayer_perceptron create_multilayer_perceptron(struct genome* genome);

void evaluate(struct multilayer_perceptron* network);

#endif