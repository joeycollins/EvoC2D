#include "creature.h"
#include "creature.h"
#include "sequence.h"
#include "innovation.h"
#include "simulation.h"
#include "creaturecontext.h"
#include "settings.h"
#include "utils.h"
#include <string.h>
#include <cglm/cglm.h>
#include <stdbool.h>

extern float creature_scale_factor;
extern struct Simulation main_simulation; //used for collision, structural innovation

void position_component_origin(struct creature* creature, struct component* component,
	struct structural_innovation_context* context);

void position_component_at(struct creature* creature, struct component* component, struct component* dest, int position, 
	struct structural_innovation_context* context);

void position_component_random(struct creature* creature, struct component* component, 
	struct structural_innovation_context* context);

void position_component_by_key(struct creature* creature, struct component* component, struct int_sequence key,
	struct structural_innovation_context* context);

struct component* add_component(struct creature* creature, struct component component);

void breed_creature(struct creature* FATHER_creature, struct creature* mother_creature, float life_span);

void breed_creature_asex(struct creature* mother, float life_span);

void assign_creature_local_positions(struct component* component, float offset_x, float offset_y, float offset_angle);
/*
Create a new creature with life_span and memory for # of component_count, which should be known
*/
struct creature create_creature(float life_span, int component_count, mat4 translation, int generation) {
	struct creature new_creature = {
		.effective_input_count = 0,
		.effective_output_count = 0,
		.life_span = life_span,
		.remaining_life_span = life_span,
		.life_stage = ALIVE,
		.generation = generation,
		.predator = false,
		.multiplicative_color = {1.0f, 1.0f, 1.0f},
		.components = {
			.capacity = component_count,
			.count = 0,
			.buffer = malloc(sizeof(struct component) * component_count),
			.realloc_amt = 0 //?
		},
		.reproduce_asex = &breed_creature_asex,
		.reproduce_sex = &breed_creature,
		.origin = NULL
	};

	generate_random_name(new_creature.name);
	//Set transform
	glm_mat4_copy(translation, new_creature.transform);
	vec3 scale = { creature_scale_factor, creature_scale_factor, 1.0f };
	glm_scale(new_creature.transform, scale);
	
	return new_creature;
}

struct int_sequence get_origin_key() {
	struct int_sequence key = {
		.count = 1,
		.capacity = 1,
		.buffer = malloc(sizeof(int)),
		.realloc_amt = 0
	};
	key.buffer[0] = 0;
	return key;
}

void copy_io_ids(struct component* component, struct int_sequence ids) {
	memcpy(component->io_component.ids, ids.buffer, sizeof(int) * ids.count);
}

void assign_key(struct component* component, struct int_sequence key) {
	component->key = key;
}

void assign_structural_innovation_number(struct component* component, int number) {
	component->structural_innovation_number = number;
}

void position_component_origin(struct creature* creature, struct component* component,
	struct structural_innovation_context* context) {
	struct int_sequence key = get_origin_key();

	struct add_structural_innovation innovation = get_add_structural_innovation(context,
		component, key);
	
	assign_structural_innovation_number(component, innovation.structural_innovation_number);
	copy_io_ids(component, innovation.ids);
	assign_color(component, innovation.color);
	assign_key(component, key);

	creature->origin = component;
}


struct component* add_component(struct creature* creature, struct component component) { //this should be moved to component.c probably
	component.this_creature = creature;
	struct component* addr = sequence_add_component(&creature->components, component);
	if (component.io_type == INPUT) {
		creature->effective_input_count += component.io_component.vector_size;
		creature->real_input_count++;
	}
	else if (component.io_type == OUTPUT) {
		creature->effective_output_count += component.io_component.vector_size;
		creature->real_output_count++;
	}
	if (addr->evolution_effect != NULL) { //call evolution effect
		addr->evolution_effect(creature);
	}

	attach_collider(addr, true);
	return addr;
}

void position_component_at(struct creature* creature, struct component* component, struct component* dest, int position,
	struct structural_innovation_context* context) {
	struct int_sequence key = {
		.capacity = dest->key.count + 1,
		.buffer = malloc((dest->key.count + 1) * sizeof(int)),
		.count = 0,
		.realloc_amt = 0
	};

	for (int j = 0; j < dest->key.count; j++) {
		sequence_add_int(&key, dest->key.buffer[j]);
	}

	if (position == -1) {
		for (int i = 0; i < MAX_CHILDREN; i++) { //adds to first open slot
			if (dest->child_exists[i] == false) {
				sequence_add_int(&key, i);
				dest->children[i] = component;
				dest->children_count++;
				dest->child_exists[i] = true;
				break;
			}
		}
	}
	else {
		sequence_add_int(&key, position);
		dest->children[position] = component;
		if (dest->child_exists[position] == false) {
			dest->children_count++;
			dest->child_exists[position] = true;
		}
		else {
			printf("A component was replaced\n");
		}
	}

	struct add_structural_innovation innovation = get_add_structural_innovation(context,
		component, key);

	assign_structural_innovation_number(component, innovation.structural_innovation_number);
	assign_key(component, key);
	assign_color(component, innovation.color);
	copy_io_ids(component, innovation.ids);
}

void position_component_random(struct creature* creature, struct component* component,
	struct structural_innovation_context* context) {
	int random = rand();
	int current_comps = creature->components.count;
	int comp_idx = random % current_comps;
	struct component* add_to = creature->components.buffer + comp_idx;
	position_component_at(creature, component, add_to, -1, context);
}

void position_component_by_key(struct creature* creature, struct component* component, struct int_sequence key,
	struct structural_innovation_context* context) {
	struct component* current = creature->origin;
	for (int i = 1; i < key.count - 1; i++) {
		current = current->children[key.buffer[i]];
	}
	position_component_at(creature, component, current, key.buffer[key.count - 1], context);
}


void create_simple_creature(struct creature* creature_base) {
	mat4 translation;
	get_random_translation(0, 0, CREATURE_SPAWN_RADIUS, translation);
	(*creature_base) = create_creature(INITIAL_CREATURE_LIFE_SPAN, 5, translation, 0);

	struct component origin = create_component(FOOD_SENSOR);
	struct component* origin_addr = add_component(creature_base, origin);
	position_component_origin(creature_base, origin_addr, &main_simulation.main_structural_innovation_context);

	
	struct component thruster = create_component(THRUSTER);
	struct component* thruster_addr = add_component(creature_base, thruster);
	position_component_at(creature_base, thruster_addr, creature_base->origin, 2, &main_simulation.main_structural_innovation_context);


	struct component sex = create_component(SEXUAL_REPRO); 
	struct component* sex_addr = add_component(creature_base, sex);
	position_component_at(creature_base, sex_addr, thruster_addr, 1, &main_simulation.main_structural_innovation_context);
	
	/*
	struct component cresen = create_component(CREATURE_SENSOR);
	struct component* cresen_addr = add_component(creature_base, cresen);
	position_component_at(creature_base, cresen_addr, sex_addr, 3, &main_simulation.main_structural_innovation_context);
	*/
	struct component asensor = create_component(ABYSS_SENSOR);
	struct component* asensor_addr = add_component(creature_base, asensor);
	position_component_at(creature_base, asensor_addr, sex_addr, -1, &main_simulation.main_structural_innovation_context);
	/*
	struct component asex = create_component(ASEX_REPRO);
	struct component* asex_addr = add_component(creature_base, asex);
	position_component_at(creature_base, asex_addr, thruster_addr, -1, &main_simulation.main_structural_innovation_context);
	*/
	assign_creature_local_positions(origin_addr, 0, 0, 0); // assign local position since we wont create a shape for all creatures spawned
	creature_base->genome = create_genome(creature_base);
	initial_mutate(&creature_base->genome, &main_simulation.main_innovation_context);
	creature_base->network = create_linked_network(&creature_base->genome);
}

void create_simple_creature_2(struct creature* creature_base) {
	mat4 translation;
	get_random_translation(0, 0, CREATURE_SPAWN_RADIUS, translation);
	(*creature_base) = create_creature(INITIAL_CREATURE_LIFE_SPAN, 5, translation, 0);

	struct component origin = create_component(FOOD_SENSOR);
	struct component* origin_addr = add_component(creature_base, origin);
	position_component_origin(creature_base, origin_addr, &main_simulation.main_structural_innovation_context);


	struct component thruster = create_component(THRUSTER);
	struct component* thruster_addr = add_component(creature_base, thruster);
	position_component_at(creature_base, thruster_addr, creature_base->origin, 2, &main_simulation.main_structural_innovation_context);

	struct component rot = create_component(ROTATOR);
	struct component* rot_addr = add_component(creature_base, rot);
	position_component_at(creature_base, rot_addr, creature_base->origin, 4, &main_simulation.main_structural_innovation_context);

	
	struct component asex = create_component(ASEX_REPRO);
	struct component* asex_addr = add_component(creature_base, asex);
	position_component_at(creature_base, asex_addr, creature_base->origin, 6, &main_simulation.main_structural_innovation_context);

	struct component em = create_component(ENERGY_METER);
	struct component* em_addr = add_component(creature_base, em);
	position_component_at(creature_base, em_addr, origin_addr, 3, &main_simulation.main_structural_innovation_context);

	assign_creature_local_positions(origin_addr, 0, 0, 0); // assign local position since we wont create a shape for all creatures spawned
	creature_base->genome = create_genome(creature_base);
	initial_mutate(&creature_base->genome, &main_simulation.main_innovation_context);
	creature_base->network = create_linked_network(&creature_base->genome);
}

void create_simple_creature_3(struct creature* creature_base) {
	mat4 translation;
	get_random_translation(0, 0, CREATURE_SPAWN_RADIUS, translation);
	(*creature_base) = create_creature(INITIAL_CREATURE_LIFE_SPAN, 2, translation, 0);

	struct component origin = create_component(FOOD_SENSOR);
	struct component* origin_addr = add_component(creature_base, origin);
	position_component_origin(creature_base, origin_addr, &main_simulation.main_structural_innovation_context);


	struct component thruster = create_component(THRUSTER);
	struct component* thruster_addr = add_component(creature_base, thruster);
	position_component_at(creature_base, thruster_addr, creature_base->origin, 2, &main_simulation.main_structural_innovation_context);

	
	assign_creature_local_positions(origin_addr, 0, 0, 0); // assign local position since we wont create a shape for all creatures spawned
	creature_base->genome = create_genome(creature_base);
	initial_mutate(&creature_base->genome, &main_simulation.main_innovation_context);
	creature_base->network = create_linked_network(&creature_base->genome);
}

void free_creature(struct creature* creature) {
	free_linked_network(&creature->network);
	free_genome(&creature->genome);
	for (int i = 0; i < creature->components.count; i++) {
		free_component(creature->components.buffer + i);
	}
	free(creature->components.buffer);
}

//component struct innov numbers are equal
bool components_are_equivalent(struct component* comp1, struct component* comp2) { 
	return comp1->structural_innovation_number == comp2->structural_innovation_number;
}


void mutate_structural(struct creature* creature) {
	float roll = rand_flt();
	enum activity_type activity = (enum activity_type)(rand() % ACTIVITY_COUNT);
	struct component new_component = create_component(activity);
	struct component* new_component_addr = add_component(creature, new_component);
	position_component_random(creature, new_component_addr, &main_simulation.main_structural_innovation_context);
}


void breed_creature_rec(struct component* current_father_component, struct component* current_mother_component, 
	struct creature* dest_creature, bool at_origin);

void breed_creature(struct creature* FATHER_creature, struct creature* mother_creature, float life_span) {
	struct creature* dest = add_to_context(&main_simulation.main_creature_context);
	mat4 translation;
	get_translation_matrix(FATHER_creature->transform, translation);
	int mutate_structurally = 0;
	if (rand_flt() < STRUCTURAL_MUTATION_CHANCE) {
		mutate_structurally = 1;
	}
	//could count before. currently just sac the memory, shouldnt be an issue
	//reallocing is not safe for component count since the breed rec fn will set the origin and stored component pointers for component children.
	*dest = create_creature(life_span,
		FATHER_creature->components.count + mother_creature->components.count + mutate_structurally, translation, FATHER_creature->generation + 1);
	breed_creature_rec(FATHER_creature->origin, mother_creature->origin,
		dest, true);
	if (mutate_structurally == 1) {
		mutate_structural(dest);
	}

	assign_creature_local_positions(dest->origin, 0.0f, 0.0f, 0.0f);
	dest->genome = create_genome(dest);
	breed_genomes(&FATHER_creature->genome, &mother_creature->genome, &dest->genome);
	for (int i = 0; i < rand() % 3; i++) {
		mutate(&dest->genome, &main_simulation.main_innovation_context, MUTATE_CONNECTION_CHANCE, MUTATE_GENE_CHANCE, MUTATE_CONNECTION_STATE_CHANCE);
	}
	dest->network = create_linked_network(&dest->genome);
}

void breed_creature_rec(struct component* current_father_component, struct component* current_mother_component, struct creature* dest_creature,
	bool at_origin) {
	struct component new_component;
	if (current_father_component == NULL && current_mother_component == NULL) {
		return;
	}else if (current_father_component == NULL) { //disjoint
		new_component = create_component(current_mother_component->activity_type);
	//disjoint || equivalent
	}else if (current_mother_component == NULL || components_are_equivalent(current_father_component, current_mother_component)) {
		new_component = create_component(current_father_component->activity_type);		
	}
	else { //competing
		int random = rand();
		int choice = random % 2;
		if (choice == 0) {
			new_component = create_component(current_father_component->activity_type);
		}
		else {
			new_component = create_component(current_mother_component->activity_type);
		}
	}
	struct component* new_comp_addr = add_component(dest_creature, new_component);
	if (at_origin) {
		position_component_origin(dest_creature, new_comp_addr, &main_simulation.main_structural_innovation_context);
		at_origin = false;
	}
	else {
		position_component_by_key(dest_creature, new_comp_addr,
			current_father_component == NULL ? current_mother_component->key : current_father_component->key,
			&main_simulation.main_structural_innovation_context);
	}
	if (current_father_component == NULL) {
		for (int i = 0; i < MAX_CHILDREN; i++) {
			if (current_mother_component->child_exists[i]) {
				breed_creature_rec(NULL, current_mother_component->children[i], dest_creature, at_origin);
			}
		}
	}
	else if (current_mother_component == NULL) {
		for (int i = 0; i < MAX_CHILDREN; i++) {
			if (current_father_component->child_exists[i]) {
				breed_creature_rec(current_father_component->children[i], NULL, dest_creature, at_origin);
			}
		}
	}
	else {
		for (int i = 0; i < MAX_CHILDREN; i++) {
			bool mother_child_exists = current_mother_component->child_exists[i];
			bool father_child_exists = current_father_component->child_exists[i];
			if (mother_child_exists && father_child_exists) {
				breed_creature_rec(current_father_component->children[i], current_mother_component->children[i], dest_creature, at_origin);
			}
			else if (mother_child_exists) {
				breed_creature_rec(NULL, current_mother_component->children[i], dest_creature, at_origin);
			}
			else if (father_child_exists) {
				breed_creature_rec(current_father_component->children[i], NULL, dest_creature, at_origin);
			}
		}
	}
}

void breed_creature_asex_rec(struct component* current_component, struct creature* destination_creature, bool at_origin);

void breed_creature_asex(struct creature* mother, float life_span) {
	struct creature* dest = add_to_context(&main_simulation.main_creature_context);
	mat4 translation;
	get_translation_matrix(mother->transform, translation);
	
	*dest = create_creature(life_span, mother->components.count, translation, mother->generation + 1);
	breed_creature_asex_rec(mother->origin, dest, life_span);
	assign_creature_local_positions(dest->origin, 0.0f, 0.0f, 0.0f);
	dest->genome = create_genome(dest);
	copy_genome(&mother->genome, &dest->genome);
	mutate(&dest->genome, &main_simulation.main_innovation_context, MUTATE_CONNECTION_CHANCE, MUTATE_GENE_CHANCE, MUTATE_CONNECTION_STATE_CHANCE);
	dest->network = create_linked_network(&dest->genome);
}

void breed_creature_asex_rec(struct component* current_component, struct creature* dest_creature,bool at_origin) {
	//
	struct component new_component = create_component(current_component->activity_type);
	struct component* new_comp_addr = add_component(dest_creature, new_component);
	if (at_origin) {
		position_component_origin(dest_creature, new_comp_addr, &main_simulation.main_structural_innovation_context);
		at_origin = false;
	}
	else {
		position_component_by_key(dest_creature, new_comp_addr, current_component->key,
			&main_simulation.main_structural_innovation_context);
	}
	for (int i = 0; i < MAX_CHILDREN; i++) {
		bool child_exists = current_component->child_exists[i];
		if (child_exists) {
			breed_creature_asex_rec(current_component->children[i], dest_creature, at_origin);
		}
	}
}

void evaluate_creature_collision(struct creature* creature) {
	int component_count = creature->components.count;
	for (int i = 0; i < component_count; i++) {
		struct component* component = creature->components.buffer + i;
		if (component->collider.enabled) {
			vec2 pos;
			get_component_position(component, pos);
			for (int j = 0; j < main_simulation.main_abyss_context.abysses_count; j++) {
				bool is_coll = component->collider.collision_fn(pos[0], pos[1], COMPONENT_RADIUS * creature_scale_factor,
					main_simulation.main_abyss_context.abysses[j].transform[3][0], main_simulation.main_abyss_context.abysses[j].transform[3][1], 400.0f);
				if (is_coll) {
					creature->remaining_life_span = -100.0f;
					return;
				}
			}

			for (int j = 0; j < main_simulation.main_food_context.food_count; j++) {
				if (!main_simulation.main_food_context.food[j].alive) { continue; }
				bool is_coll = component->collider.collision_fn(pos[0], pos[1], COMPONENT_RADIUS * creature_scale_factor,
					main_simulation.main_food_context.food[j].transform[3][0], main_simulation.main_food_context.food[j].transform[3][1], 50.0f);
				if (is_coll) {
					creature->remaining_life_span += 5.0f;
					main_simulation.main_food_context.food[j].alive = false;
					break;
				}
			}

		}
	}
}

void evaluate_creature_network(struct creature* creature) {
	evaluate_linked_network(&creature->network, &tanhf);
}

//add an ondeath callback later instead of returning boolean
bool evaluate_creature_life(struct creature* creature) {
	creature->remaining_life_span -= main_simulation.delta_time;
	if (creature->remaining_life_span < 0.0f) {
		creature->life_stage = DEAD;
		return false;
	}
	return true;
}

void update_creature(struct creature* creature) {
	evaluate_creature_collision(creature);
	evaluate_creature_network(creature);
	if (!evaluate_creature_life(creature)) {
		main_simulation.main_creature_context.alive_creatures--;
	}
}

void assign_creature_local_positions(struct component* component, float offset_x, float offset_y, float offset_angle) {
	mat4 local_transform;
	glm_mat4_identity(local_transform);
	local_transform[3][0] = offset_x;
	local_transform[3][1] = offset_y;
	glm_mat4_copy(local_transform, component->local_transform);
	vec2 position_offset = { offset_x, offset_y };
	glm_vec2_copy(position_offset, component->local_position);
	for (int i = 0; i < MAX_CHILDREN; i++) {
		if (component->child_exists[i] == false) { continue; }

		float angle = (float)(i * GROWTH_ANGLE + offset_angle + GROWTH_ANGLE / 2);
		float x = (2 * COMPONENT_RADIUS + GROWTH_RADIUS) * (float)cos(angle) + position_offset[0];
		float y = (2 * COMPONENT_RADIUS + GROWTH_RADIUS) * (float)sin(angle) + position_offset[1];

		assign_creature_local_positions(component->children[i], x, y, (float)(angle + M_PI));
	}
}
