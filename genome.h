#ifndef GENOME_H_INCLUDED
#define GENOME_H_INCLUDED

#include "connection.h"
#include "gene.h"
#include <stdbool.h>

#define INITIAL_CONNECTION_CAPACITY 700
#define INITIAL_HIDDEN_CAPACITY 300

struct creature; struct innovation_context; struct linked_list;

struct genome {
	int layers;
	struct gene_pointer_sequence input_genes;
	struct gene_pointer_sequence hidden_genes;
	struct gene_pointer_sequence output_genes;
	struct connection_sequence connections;
	struct creature* this_creature;
};

struct genome create_genome(struct creature* creature);

void free_genome(struct genome* genome);

void mutate_add_connection(struct genome* genome, struct innovation_context* context);

void mutate_add_gene(struct genome* genome, struct innovation_context* context);

void mutate(struct genome* genome, struct innovation_context* context, float add_connection_chance, 
	float add_gene_chance, float innovation_mutation_chance);

void initial_mutate(struct genome* genome, struct innovation_context* context);

void breed_genomes(struct genome* father, struct genome* mother, struct genome* child_base);

void copy_genome(struct genome* genome, struct genome* dest);

#endif