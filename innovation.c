#include "innovation.h"
#include "sequence.h"
#include <stdlib.h>

struct innovation_context get_new_innovation_context() {
	struct innovation_context new_context = {
		.innovation_number = 1,
		.add_connection_innovations = {
			.capacity = INITIAL_INNOVATIONS_CAPACITY,
			.count = 0,
			.buffer = calloc(INITIAL_INNOVATIONS_CAPACITY, sizeof(struct add_connection_innovation)),
			.realloc_amt = INNOVATION_REALLOC_AMT
		},
		.add_gene_innovations = {
			.capacity = INITIAL_INNOVATIONS_CAPACITY,
			.count = 0,
			.buffer = calloc(INITIAL_INNOVATIONS_CAPACITY, sizeof(struct add_gene_innovation)),
			.realloc_amt = INNOVATION_REALLOC_AMT
		}
	};

	return new_context;
}

int get_new_innovation_number(struct innovation_context* context) {
	int new_number = context->innovation_number;
	context->innovation_number++;
	return new_number;
}

struct add_connection_innovation get_add_connection_innovation(struct innovation_context* context, 
	int first_gene_id, int second_gene_id) {

	for (int i = 0; i < context->add_connection_innovations.count; i++) {
		if (context->add_connection_innovations.buffer[i].first_gene_id == first_gene_id &&
			context->add_connection_innovations.buffer[i].second_gene_id == second_gene_id) {
			return context->add_connection_innovations.buffer[i];
		}
	}

	struct add_connection_innovation new_innovation = {
		.first_gene_id = first_gene_id,
		.second_gene_id = second_gene_id,
		.innovation_number = get_new_innovation_number(context)
	};

	sequence_add_connection_innovation(&context->add_connection_innovations, new_innovation);

	return new_innovation;
}

struct add_gene_innovation get_add_gene_innovation(struct innovation_context* context, int connection_split_innov) {
	for (int i = 0; i < context->add_gene_innovations.count; i++) {
		if (context->add_gene_innovations.buffer[i].conn_split_innov == connection_split_innov) {
			return context->add_gene_innovations.buffer[i];
		}
	}

	struct add_gene_innovation new_innovation = {
		.conn_split_innov = connection_split_innov,
		.innovation_number_1 = get_new_innovation_number(context),
		.innovation_number_2 = get_new_innovation_number(context)
	};

	sequence_add_gene_innovation(&context->add_gene_innovations, new_innovation);

	return new_innovation;
}