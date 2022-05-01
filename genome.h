#ifndef GENOME_H
#define GENOME_H 

#include "gene.h"
#include "connection.h"
#include "innovation.h"
#include <stdbool.h>

#define INITIAL_CONNECTION_CAPACITY 1000
#define INITIAL_HIDDEN_CAPACITY 100

struct genome {
	struct gene_sequence input_genes;
	struct gene_sequence hidden_genes;
	struct gene_sequence output_genes;
	struct connection_sequence connections;

};

#endif