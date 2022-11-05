#include "sequence.h"
#include <stdlib.h>

#define SEQUENCE_ADD(S, T, N) \
	T* sequence_add_##N(S* sequence, T element){ \
		if (sequence->count + 1 > sequence->capacity) { \
			sequence->capacity += sequence->realloc_amt; \
			T* new_buffer = realloc(sequence->buffer, sequence->capacity * sizeof(T));\
			if (new_buffer != NULL){ \
				sequence->buffer = new_buffer; \
			} else { \
				perror("New sequence buffer is NULL in add_sequence"); \
			}\
		} \
		sequence->buffer[sequence->count] = element; \
		T* addr = sequence->buffer + sequence->count; \
		sequence->count++; \
		return addr;\
	} \

#define UNBOUNDED_SEQUENCE_ADD(S, T, N) \
	T** unbounded_sequence_add_##N(S* sequence, T element){ \
		if (sequence->count + 1 > sequence->capacity) { \
			sequence->capacity += sequence->realloc_amt; \
			T* new_buffer = realloc(sequence->buffer, sequence->capacity * sizeof(T));\
			if (new_buffer != NULL){ \
				sequence->buffer = new_buffer; \
			} else { \
				perror("New unbound sequence buffer is NULL in add_sequence"); \
			}\
		}\
		T* element_addr = malloc(sizeof(T));\
		(*element_addr) = element; \
		T** new_addr = sequence->buffer + sequence->count;\
		(*new_addr) = element_addr;\
		sequence->count++; \
		return new_addr;\
	}\

#define UNBOUNDED_SEQUENCE_FREE(S, T, N) \
	void unbounded_sequence_free_##N(S* sequence) { \
		for (int i = 0; i < sequence->count; i++){ \
			free(*(sequence->buffer + i)); \
		}\
		free(sequence->buffer); \
	}\


#define SEQUENCE_WHERE(S, T, N) \
	T* sequence_where_##N(S* sequence, bool (*condition)(T*)){ \
		for (int i = 0; i < sequence->count; i++){ \
			if ((*condition)(sequence->buffer + i)) { \
				return sequence->buffer + i; \
			} \
		} \
		return NULL; \
	}\

SEQUENCE_ADD(struct add_connection_innovation_sequence, struct add_connection_innovation, connection_innovation)
SEQUENCE_ADD(struct add_gene_innovation_sequence, struct add_gene_innovation, gene_innovation)
SEQUENCE_ADD(struct gene_sequence, struct gene, gene)
SEQUENCE_ADD(struct connection_sequence, struct connection, connection)
SEQUENCE_ADD(struct component_sequence, struct component, component)
SEQUENCE_ADD(struct int_sequence, int, int)
SEQUENCE_ADD(struct add_structural_innovation_sequence, struct add_structural_innovation, structural_innovation)
SEQUENCE_ADD(struct gene_pointer_sequence, struct gene*, gene_pointer)




SEQUENCE_WHERE(struct gene_sequence, struct gene, gene)
