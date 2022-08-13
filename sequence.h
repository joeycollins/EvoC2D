#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "intsequence.h"
#include "innovation.h"
#include "gene.h"
#include "connection.h"
#include "component.h"
#include <stdbool.h>

#define SEQUENCE_ADD_HEADER(S, T, N) \
	T* sequence_add_##N(S* sequence, T element); \

#define SEQUENCE_WHERE_HEADER(S, T, N) \
	T* sequence_where_##N(S* sequence, bool (*condition)(T*)); \

SEQUENCE_ADD_HEADER(struct add_connection_innovation_sequence, struct add_connection_innovation, connection_innovation)
SEQUENCE_ADD_HEADER(struct add_gene_innovation_sequence, struct add_gene_innovation, gene_innovation)
SEQUENCE_ADD_HEADER(struct gene_sequence, struct gene, gene)
SEQUENCE_ADD_HEADER(struct connection_sequence, struct connection, connection)
SEQUENCE_ADD_HEADER(struct component_sequence, struct component, component)
SEQUENCE_ADD_HEADER(struct int_sequence, int, int)
SEQUENCE_ADD_HEADER(struct add_structural_innovation_sequence, struct add_structural_innovation, structural_innovation)



SEQUENCE_WHERE_HEADER(struct gene_sequence, struct gene, gene)
#endif // !SEQUENCE_INCLUDED