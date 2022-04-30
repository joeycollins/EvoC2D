#include "innovation.h"
#include "sequence.h"
#include <stdlib.h>

struct innovation_context get_new_innovation_context() {
	struct innovation_context new_context = {
		.innovation_number = 1,
		.add_connection_innovations = calloc(INITIAL_INNOVATIONS_CAPACITY, sizeof(struct add_connection_innovation)),
		.add_connection_innovations_capacity = INITIAL_INNOVATIONS_CAPACITY,
		.add_connection_innovations_count = 0,
		.add_gene_innovations = calloc(INITIAL_INNOVATIONS_CAPACITY, sizeof(struct add_gene_innovation)),
		.add_gene_innovations_capacity = INITIAL_INNOVATIONS_CAPACITY,
		.add_gene_innovations_count = 0
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

	for (int i = 0; i < context->add_connection_innovations_count; i++) {
		if (context->add_connection_innovations[i].first_gene_id == first_gene_id &&
			context->add_connection_innovations[i].second_gene_id == second_gene_id) {
			return context->add_connection_innovations[i];
		}
	}

	struct add_connection_innovation new_innovation = {
		.first_gene_id = first_gene_id,
		.second_gene_id = second_gene_id,
		.innovation_number = get_new_innovation_number(context)
	};

	sequence_add_connection_innovation(context->add_connection_innovations, new_innovation, &context->add_connection_innovations_count,
		&context->add_connection_innovations_capacity);

	return new_innovation;
}