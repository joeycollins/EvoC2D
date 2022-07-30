#ifndef GENOME_H
#define GENOME_H 

#include "linkedlist.h"
#include "connection.h"
#include "innovation.h"
#include "creature.h"
#include "gene.h"
#include <stdbool.h>

#define INITIAL_CONNECTION_CAPACITY 1000
#define INITIAL_HIDDEN_CAPACITY 100

struct genome {
	int layers;
	int input_groups_count;
	int output_groups_count;
	struct linked_list* input_groups;
	struct linked_list* output_groups;
	struct gene_sequence input_genes;
	struct gene_sequence hidden_genes;
	struct gene_sequence output_genes;
	struct connection_sequence connections;
};

struct genome create_genome(struct creature* creature);

void free_genome(struct genome* genome);

void mutate_add_connection(struct genome* genome, struct innovation_context* context);

void mutate_add_gene(struct genome* genome, struct innovation_context* context);

#endif