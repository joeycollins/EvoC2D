#ifndef MLAYER_H
#define MLAYER_H

#include "genome.h"

struct multilayer_perceptron {
	void (*evaluate)(void);

};

struct multilayer_perceptron create_multilayer_perceptron(struct genome* genome);

#endif