#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "innovation.h"
#include "gene.h"
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SEQUENCE_ADD_HEADER(S, T, N) \
	T* sequence_add_##N(S* sequence, T element); \

SEQUENCE_ADD_HEADER(struct add_connection_innovation_sequence, struct add_connection_innovation, connection_innovation)
SEQUENCE_ADD_HEADER(struct add_gene_innovation_sequence, struct add_gene_innovation, gene_innovation)

#endif // !SEQUENCE_INCLUDED

