#ifndef GENE_H_INCLUDED
#define GENE_H_INCLUDED

struct component;

struct gene {
	//structure
	int id;
	int distance;
	struct component* component;
	//evalution
	float value;
	int active_incoming_connections;
	int incoming_connections_executed;
};

struct gene_sequence {
	int capacity;
	int count;
	struct gene* buffer;
	int realloc_amt;
};

//unbounded sequences allow for safe realloc
struct gene_pointer_sequence { 
	int capacity;
	int count;
	struct gene** buffer;
	int realloc_amt;
};

struct gene create_gene(int id, int distance, struct component* component);
#endif