#ifndef INCLUDE_INNOVATION
#define INCLUDE_INNOVATION

#define INITIAL_INNOVATIONS_CAPACITY 300
#define INNOVATION_REALLOC_AMT 500

struct add_connection_innovation {
	int first_gene_id;
	int second_gene_id;
	int innovation_number;
};

struct add_gene_innovation {
	int conn_split_innov;
	int innovation_number_1;
	int innovation_number_2;
};

struct add_connection_innovation_sequence {
	int capacity;
	int count;
	struct add_connection_innovation* buffer;
	int realloc_amt;
};

struct add_gene_innovation_sequence {
	int capacity;
	int count;
	struct add_gene_innovation* buffer;
	int realloc_amt;
};

struct innovation_context {
	int innovation_number;
	struct add_connection_innovation_sequence add_connection_innovations;
	struct add_gene_innovation_sequence add_gene_innovations;
};

struct innovation_context get_new_innovation_context();

void free_innovation_context(struct innovation_context* context);

struct add_gene_innovation get_add_gene_innovation(struct innovation_context* context, int connection_split_innov);

struct add_connection_innovation get_add_connection_innovation(struct innovation_context* context,
	int first_gene_id, int second_gene_id);

#endif // !INCLUDE_INNOVATION
