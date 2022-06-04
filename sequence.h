#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "innovation.h"
#include "gene.h"
#include "connection.h"
#include "component.h"

#define SEQUENCE_ADD_HEADER(S, T, N) \
	T* sequence_add_##N(S* sequence, T element); \

SEQUENCE_ADD_HEADER(struct add_connection_innovation_sequence, struct add_connection_innovation, connection_innovation)
SEQUENCE_ADD_HEADER(struct add_gene_innovation_sequence, struct add_gene_innovation, gene_innovation)
SEQUENCE_ADD_HEADER(struct gene_sequence, struct gene, gene)
SEQUENCE_ADD_HEADER(struct connection_sequence, struct connection, connection)
SEQUENCE_ADD_HEADER(struct component_sequence, struct component, component)

#endif // !SEQUENCE_INCLUDED

