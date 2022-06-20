#ifndef MLAYER_H
#define MLAYER_H

#include "genome.h"
#include "component.h"

struct network_matrix {
    float* buffer;
	int rows;
    int columns;
};

struct multilayer_perceptron {
	int input_vector_size;
	int input_components_count;
	struct component** input_components;
	int output_vector_size;
	int output_components_count;
	struct component** output_components;
	int matrices_count;
	struct network_matrix* matrices;
	float (*activation)(float);
};

struct multilayer_perceptron create_multilayer_perceptron(struct genome* genome);

void evaluate(struct multilayer_perceptron* network);

#endif