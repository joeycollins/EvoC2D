#ifndef SEQUENCE_INCLUDED
#define SEQUENCE_INCLUDED

#include "innovation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SEQUENCE_ADD_HEADER(T, N) \
	T* sequence_add_##N(T* sequence, T element, int* count, int* capacity); \

SEQUENCE_ADD_HEADER(struct add_connection_innovation, connection_innovation)
SEQUENCE_ADD_HEADER(struct add_gene_innovation, gene_innovation)

#endif // !SEQUENCE_INCLUDED

