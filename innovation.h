#ifndef INCLUDE_INNOVATION
#define INCLUDE_INNOVATION

#define INITIAL_INNOVATIONS_CAPACITY 300

struct innovation_context {
	int innovation_number;
	struct add_connection_innovation* add_connection_innovations;
	int add_connection_innovations_count;
	int add_connection_innovations_capacity;
	struct add_gene_innovation* add_gene_innovations;
	int add_gene_innovations_count;
	int add_gene_innovations_capacity;
};

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

struct innovation_context get_new_innovation_context();

#endif // !INCLUDE_INNOVATION
