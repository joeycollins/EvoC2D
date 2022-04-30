#include "sequence.h"

#define SEQUENCE_ADD(T, N) \
	T* sequence_add_##N(T* sequence, T element, int* count, int* capacity){ \
		if (*count + 1 == *capacity) { \
			*capacity += 1000; \
			T* new_sequence = realloc(sequence, *capacity * sizeof(T));\
			if (new_sequence != NULL){ \
				sequence = new_sequence; \
			} \
		} \
		sequence[*count] = element; \
		T* addr = sequence + *count; \
		(*count)++; \
		return addr;\
	} \

SEQUENCE_ADD(struct add_connection_innovation, connection_innovation)
SEQUENCE_ADD(struct add_gene_innovation, gene_innovation)