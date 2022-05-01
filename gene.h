#ifndef GENE_H
#define GENE_H

struct gene {
	int id;
	int distance;
};

struct gene_sequence {
	unsigned int capacity;
	unsigned int count;
	struct gene* buffer;
	unsigned int realloc_amt;
};
#endif