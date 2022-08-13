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

struct structural_innovation_context create_structural_innovation_context() {
	struct structural_innovation_context new_context = {
		.innovation_number = -1,
		.add_structural_innovations = {
			.capacity = INITIAL_INNOVATIONS_CAPACITY,
			.count = 0,
			.buffer = calloc(INITIAL_INNOVATIONS_CAPACITY, sizeof(struct add_structural_innovation)),
			.realloc_amt = INNOVATION_REALLOC_AMT
		}
	};
	return new_context;
}

void free_innovation_context(struct innovation_context* context) {
	free(context->add_connection_innovations.buffer);
	free(context->add_gene_innovations.buffer);
}

void free_structural_innovation_context(struct structural_innovation_context* context) {
	for (int i = 0; i < context->add_structural_innovations.count; i++) {
		free(context->add_structural_innovations.buffer[i].key.buffer);
		free(context->add_structural_innovations.buffer[i].ids.buffer);
	}
	free(context->add_structural_innovations.buffer);
}

int get_new_innovation_number(struct innovation_context* context) {
	int new_number = context->innovation_number;
	context->innovation_number++;
	return new_number;
}

int get_new_structural_innovation_number(struct structural_innovation_context* context) {
	int new_number = context->innovation_number;
	context->innovation_number--;
	return new_number;
}

void get_random_color(float* rdest, float* gdest, float* bdest) {
	float x = rand(), y = rand(), z = rand();
	*rdest = x / RAND_MAX;
	*gdest = y / RAND_MAX;
	*bdest = z / RAND_MAX;
}

bool keys_are_identical(struct int_sequence* key1, struct int_sequence* key2) {
	if (key1->count != key2->count) { return false; }
	for (int i = 0; i < key1->count; i++) {
		if (key1->buffer[i] != key2->buffer[i]) {
			return false;
		}
	}
	return true;
}

struct add_structural_innovation get_add_structural_innovation(struct structural_innovation_context* context,
	enum activity_type activity_type, struct int_sequence key) {

	for (int i = 0; i < context->add_structural_innovations.count; i++) {
		if (activity_type == context->add_structural_innovations.buffer[i].activity_type) {
			if (keys_are_identical(&key, &context->add_structural_innovations.buffer[i].key)) {
				return context->add_structural_innovations.buffer[i];
			}
		}
	}

	int vector_count;
	switch (activity_type) {
	case THRUSTER:
		vector_count = 2;
		break;
	case FOOD_SENSOR:
		vector_count = 2;
		break;
	case ROTATOR:
		vector_count = 1;
		break;
	case GPS:
		vector_count = 2;
		break;
	default:
		vector_count = 1;
		break;
	}
	
	float r, g, b;
	get_random_color(&r, &b, &g); //get a new color
	
	struct add_structural_innovation new_innovation = {
		.activity_type = activity_type,
		.key = key,
		.ids = {
			.count = vector_count,
			.capacity =vector_count,
			.buffer = malloc(sizeof(int) * vector_count),
			.realloc_amt = 0
		},
		.color = {r, g, b}
	};

	for (int i = 0; i < vector_count; i++) {
		new_innovation.ids.buffer[i] = get_new_structural_innovation_number(context);
	}

	sequence_add_structural_innovation(&context->add_structural_innovations, new_innovation);
	return new_innovation;
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