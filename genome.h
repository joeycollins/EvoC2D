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


struct genome get_new_genome(unsigned int inputs, unsigned int outputs);

void free_genome(struct genome* genome);

void mutate_add_connection(struct genome* genome, struct innovation_context* context, bool allow_recurrent, unsigned int seed);

void mutate_add_gene(struct genome* genome, struct innovation_context* context, unsigned int seed);

#endif