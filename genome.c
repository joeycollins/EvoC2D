#include "genome.h"
#include "sequence.h"
#include "creature.h"
#include "innovation.h"
#include "component.h"
#include "linkedlist.h"
#include "utils.h"
#include <stdlib.h>

struct genome create_genome(struct creature* creature) {
	int input_genes_count = creature->effective_input_count, output_genes_count = creature->effective_output_count;
	
	struct genome new_genome = {
		.connections = {
			.count = 0,
			.capacity = INITIAL_CONNECTION_CAPACITY,
			.buffer = calloc(INITIAL_CONNECTION_CAPACITY, sizeof(struct connection)),
			.realloc_amt = INITIAL_CONNECTION_CAPACITY
		},
		.input_genes = {
			.count = 0,
			.capacity = input_genes_count,
			.buffer = calloc(input_genes_count, sizeof(struct gene*)),
			.realloc_amt = input_genes_count
		},
		.hidden_genes = { 
			.count = 0,
			.capacity = INITIAL_HIDDEN_CAPACITY,
			.buffer = calloc(INITIAL_HIDDEN_CAPACITY, sizeof(struct gene*)),
			.realloc_amt = 1
		},
		.output_genes = {
			.count = 0,
			.capacity = output_genes_count,
			.buffer = calloc(output_genes_count, sizeof(struct gene*)),
			.realloc_amt = output_genes_count
		},
		.layers = 1,
		.this_creature = creature
	};

	for (int i = 0; i < creature->components.count; i++) {
		struct component* this_component = creature->components.buffer + i;
		enum component_type this_component_type = this_component->io_type;
		int gene_distance = this_component_type == INPUT ? 0 : 1;
		int new_genes_needed = this_component->io_component.vector_size;
		struct gene_sequence component_genes = {
			.capacity = new_genes_needed,
			.count = new_genes_needed,
			.buffer = malloc(sizeof(struct gene) * new_genes_needed),
			.realloc_amt = 0
		};

		for (int j = 0; j < new_genes_needed; j++) {
			int id = this_component->io_component.ids[j];
			struct gene new_gene = create_gene(id, gene_distance, this_component);
			component_genes.buffer[j] = new_gene;
			if (this_component_type == INPUT) {
				sequence_add_gene_pointer(&new_genome.input_genes, component_genes.buffer + j);
			}
			else if (this_component_type == OUTPUT) {
				sequence_add_gene_pointer(&new_genome.output_genes, component_genes.buffer + j);
			}
		}
		this_component->genes = component_genes;
	}

	return new_genome;
}

void free_genome(struct genome* genome) {
	free(genome->connections.buffer);
	free(genome->input_genes.buffer);
	free(genome->output_genes.buffer);
	for (int i = 0; i < genome->hidden_genes.count; i++) {
		free(genome->hidden_genes.buffer[i]);
	}
	free(genome->hidden_genes.buffer);
}

void mutate_add_connection(struct genome* genome, struct innovation_context* context) {
	int inputs_count = genome->input_genes.count;
	int hidden_count = genome->hidden_genes.count;
	int output_count = genome->output_genes.count;
	int possible_first_count = inputs_count + hidden_count;
	if (possible_first_count == 0) {
		return;
	}
	int random = rand();
	int first_choice = random % (possible_first_count);
	struct gene* first_gene;
	if (first_choice < inputs_count) {
		first_gene = genome->input_genes.buffer[first_choice];
	}
	else {
		first_gene = genome->hidden_genes.buffer[first_choice - inputs_count];
	}

	int existing_connections_count = 0;
	int* existing_connections = malloc(genome->connections.count * sizeof(int));
	for (int i = 0; i < genome->connections.count; i++) {
		if (genome->connections.buffer[i].first_gene == first_gene) {
			existing_connections[existing_connections_count] = genome->connections.buffer[i].second_gene->id;
			existing_connections_count++;
		}
	}

	int possible_second_count = 0;
	int max_possible_second_count = hidden_count + output_count;
	struct gene** possible_second_genes = malloc(max_possible_second_count * sizeof(struct gene*));
	if (possible_second_genes == NULL) { return;}
	for (int i = 0; i < max_possible_second_count; i++) {
		struct gene* gene;
		if (i < hidden_count) {
			gene = genome->hidden_genes.buffer[i];
		}
		else {
			gene = genome->output_genes.buffer[i - hidden_count];
		}
		if (gene->distance <= first_gene->distance) {
			continue;
		}
		bool exists = false;
		for (int j = 0; j < existing_connections_count; j++) {
			if (gene->id == existing_connections[j]) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			possible_second_genes[possible_second_count] = gene;
			possible_second_count++;
		}
	}
	free(existing_connections);

	if (possible_second_count == 0) { 
		free(possible_second_genes);
		return; 
	}
	random = rand();
	int second_choice = random % possible_second_count;
	struct gene* second_gene = possible_second_genes[second_choice];
	free(possible_second_genes);

	
	float connections_weight = rand_flt();
	struct add_connection_innovation innovation = get_add_connection_innovation(context, first_gene->id, second_gene->id);
	struct connection new_connection = {
		.innovation_number = innovation.innovation_number,
		.first_gene = first_gene,
		.second_gene = second_gene,
		.weight = connections_weight,
		.enabled = true,
		.split = false
	};
	sequence_add_connection(&genome->connections, new_connection);

	//update gene info
	second_gene->active_incoming_connections++;
}

bool contains_int(int* sequence, int count, int element) {
	for (int i = 0; i < count; i++) {
		if (sequence[i] == element) {
			return true;
		}
	}
	return false;
}

//push genes connected to the origin +1 distance from the input layer
void push_genes(struct connection* connections, int connection_count, int origin_id, int* expanded_ids, int* expanded_ids_count, bool recurrent) {
	if (recurrent) {
		for (int i = 0; i < connection_count; i++) {
			if (connections[i].second_gene->id == origin_id && !contains_int(expanded_ids, *expanded_ids_count, origin_id)) {
				connections[i].first_gene->distance += 1;
				expanded_ids[*expanded_ids_count] = connections[i].first_gene->id;
				(*expanded_ids_count)++;
				push_genes(connections, connection_count, connections[i].first_gene->id, expanded_ids, expanded_ids_count, recurrent);
			}
		}
	}
	else {
		for (int i = 0; i < connection_count; i++) {
			if (connections[i].first_gene->id == origin_id && !contains_int(expanded_ids, *expanded_ids_count, connections[i].second_gene->id)) {
				connections[i].second_gene->distance += 1;
				expanded_ids[*expanded_ids_count] = connections[i].second_gene->id;
				(*expanded_ids_count)++;
				push_genes(connections, connection_count, connections[i].second_gene->id, expanded_ids, expanded_ids_count, recurrent);
			}
		}
	}
}

void mutate_add_gene(struct genome* genome, struct innovation_context* context) {
	if (genome->connections.count == 0) { return; }
	int inputs_count = genome->input_genes.count;
	int hidden_count = genome->hidden_genes.count;
	int output_count = genome->output_genes.count;
	int total_count = inputs_count + hidden_count + output_count;

	int splittable_count = 0;
	struct connection** splittable_connections = calloc(genome->connections.count, sizeof(struct connection*));
	if (splittable_connections == NULL) { return; }
	for (int i = 0; i < genome->connections.count; i++) {
		if (genome->connections.buffer[i].split == false && genome->connections.buffer[i].enabled) {
			splittable_connections[splittable_count] = &genome->connections.buffer[i];
			splittable_count++;
		}
	}
	if (splittable_count == 0) { 
		free(splittable_connections);
		return;
	}

	int random = rand();
	struct connection* connection_to_split = splittable_connections[random % splittable_count];
	free(splittable_connections);
	connection_to_split->enabled = false;
	connection_to_split->split = true;
	struct add_gene_innovation innovation = get_add_gene_innovation(context, connection_to_split->innovation_number);
	struct gene new_gene = create_gene(connection_to_split->innovation_number, 0, NULL);

	if (connection_to_split->second_gene->distance - connection_to_split->first_gene->distance == 1) {
		int* expanded = calloc(total_count, sizeof(int));
		expanded[0] = connection_to_split->first_gene->id;
		int expanded_count = 1;
		push_genes(genome->connections.buffer, genome->connections.count, connection_to_split->first_gene->id, expanded, &expanded_count, false);
		free(expanded);
		genome->layers++; //this could be wrong; havent tested, 
	}
	new_gene.distance = connection_to_split->first_gene->distance + 1;
	
	struct gene* new_gene_addr = malloc(sizeof(struct gene));
	*new_gene_addr = new_gene;
	sequence_add_gene_pointer(&genome->hidden_genes, new_gene_addr);

	struct connection new_connection_1 = {
		.innovation_number = innovation.innovation_number_1,
		.first_gene = connection_to_split->first_gene,
		.second_gene = new_gene_addr,
		.weight = 1.0f,
		.split = false,
		.enabled = true
	};

	random = rand();
	struct connection new_connection_2 = {
		.innovation_number = innovation.innovation_number_1,
		.first_gene = new_gene_addr,
		.second_gene = connection_to_split->second_gene,
		.weight = rand_flt(),
		.split = false,
		.enabled = true
	};

	sequence_add_connection(&genome->connections, new_connection_1);
	sequence_add_connection(&genome->connections, new_connection_2);

	new_gene_addr->active_incoming_connections++;
}

void mutate(struct genome* genome, struct innovation_context* context, float add_connection_chance,
	float add_gene_chance, float innovation_mutation_chance) {
	float random = rand_flt();
	if (random < add_connection_chance) {
		mutate_add_connection(genome, context);
	}
	random = rand_flt();
	if (random < add_gene_chance) {
		mutate_add_gene(genome, context);
	}
	
	for (int i = 0; i < genome->connections.count; i++) {
		random = rand_flt();
		if (random < innovation_mutation_chance) {
			mutate_connection(&genome->connections.buffer[i], 0.5f,0.05f);
		}
	}
}

void initial_mutate(struct genome* genome, struct innovation_context* context) {
	
	for (int i = 0; i < 12; i++) {
		if (rand_flt() < 0.75f) {
			mutate_add_connection(genome, context);
		}
		else {
			mutate_add_gene(genome, context);
		}
	}
}

struct gene* check_hidden_gene_exists(struct genome* genome, int id) {
	for (int i = 0; i < genome->hidden_genes.count; i++) {
		if (genome->hidden_genes.buffer[i]->id == id) {
			return genome->hidden_genes.buffer[i];
		}
	}
	return NULL;
}

struct gene* check_io_gene_exists(struct genome* genome, int id){
	for (int i = 0; i < genome->input_genes.count; i++) {
		if (genome->input_genes.buffer[i]->id == id) {
			return genome->input_genes.buffer[i];
		}
	}
	for (int i = 0; i < genome->output_genes.count; i++) {
		if (genome->output_genes.buffer[i]->id == id) {
			return genome->output_genes.buffer[i];
		}
	}
	return NULL;
}

struct gene* add_bred_gene(struct genome* base, int id, int distance) {
	if (id < 0) {
		return check_io_gene_exists(base, id);
	}
	else {
		struct gene* hidden_gene = check_hidden_gene_exists(base, id);
		if (hidden_gene == NULL) {
			struct gene new_gene = create_gene(id, distance, NULL);
			struct gene* new_gene_addr = malloc(sizeof(struct gene));
			(*new_gene_addr) = new_gene;
			sequence_add_gene_pointer(&base->hidden_genes, new_gene_addr);
			return new_gene_addr;
		}
		return hidden_gene;
	}
}

struct connection* add_bred_connection(struct genome* base, int innovation_number,
	struct gene* first_gene, struct gene* second_gene, float weight, bool enabled, bool split) {
	struct connection new_connection = {
		.enabled = enabled,
		.first_gene = first_gene,
		.second_gene = second_gene,
		.innovation_number = innovation_number,
		.split = split,
		.weight = weight
	};
	second_gene->active_incoming_connections++;
	return sequence_add_connection(&base->connections, new_connection);
}
/*
* Breed a new genome. Supply a base genome with all child io genes 
*/
void breed_genomes(struct genome* father, struct genome* mother, struct genome* child_base) {
	int expanded_count = 0;

	for (int i = 0; i < father->connections.count; i++) {
		struct connection* father_connection = father->connections.buffer + i;
		int connection_innovation_number = father_connection->innovation_number;
		int first_gene_id = father_connection->first_gene->id;
		int second_gene_id = father_connection->second_gene->id;
		for (int j = 0; j < mother->connections.count; j++) {
			struct connection* mother_connection = mother->connections.buffer + j;
			if (connection_innovation_number == mother_connection->innovation_number) {
				int first_dist = max_int(father_connection->first_gene->distance, mother_connection->first_gene->distance);
				int second_dist = max_int(father_connection->second_gene->distance, mother_connection->second_gene->distance);
				struct gene* first_addr = add_bred_gene(child_base, first_gene_id, first_dist);
				struct gene* second_addr = add_bred_gene(child_base, second_gene_id, second_dist);
				if (first_addr == NULL || second_addr == NULL) {
					goto NextFConn;
				}
				float weight = rand_choice_flt(father_connection->weight, mother_connection->weight);
				add_bred_connection(child_base, connection_innovation_number, first_addr, second_addr,
					weight, father_connection->enabled, father_connection->split || mother_connection->split);
				goto NextFConn;
			}

		}

		
		struct gene* first_addr = add_bred_gene(child_base, first_gene_id, father_connection->first_gene->distance);
		struct gene* second_addr = add_bred_gene(child_base, second_gene_id, father_connection->second_gene->distance);
		if (first_addr == NULL || second_addr == NULL) {
			continue;
		}
		add_bred_connection(child_base, connection_innovation_number, first_addr, second_addr,
			father_connection->weight, father_connection->enabled, father_connection->split);
		NextFConn:;
	}

	for (int i = 0; i < mother->connections.count; i++) {
		struct connection* mother_connection = mother->connections.buffer + i;
		for (int j = 0; j < child_base->connections.count; j++) {
			if (mother_connection->innovation_number == child_base->connections.buffer[j].innovation_number) {
				goto NextMConn;
			}
		}
		
		struct gene* first_addr = add_bred_gene(child_base, mother_connection->first_gene->id, mother_connection->first_gene->distance);
		struct gene* second_addr = add_bred_gene(child_base, mother_connection->second_gene->id, mother_connection->second_gene->distance);
		if (first_addr == NULL || second_addr == NULL) {
			continue;
		}
		add_bred_connection(child_base, mother_connection->innovation_number, first_addr, second_addr,
			mother_connection->weight, mother_connection->enabled, mother_connection->split);
		NextMConn:;
	}

}

void copy_genome(struct genome* genome, struct genome* dest) {
	for (int i = 0; i < genome->connections.count; i++) {
		struct connection* connection = genome->connections.buffer + i;
		int connection_innovation_number = connection->innovation_number;
		int first_gene_id = connection->first_gene->id;
		int second_gene_id = connection->second_gene->id;
		int first_dist = connection->first_gene->distance;
		int second_dist = connection->second_gene->distance;
		struct gene* first_addr = add_bred_gene(dest, first_gene_id, first_dist);
		struct gene* second_addr = add_bred_gene(dest, second_gene_id, second_dist);
		add_bred_connection(dest, connection_innovation_number, first_addr, second_addr,
			connection->weight, connection->enabled, connection->split);
	}
}