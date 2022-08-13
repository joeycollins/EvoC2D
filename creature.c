#include "creature.h"
#include "sequence.h"
#include <string.h>
#include <cglm/cglm.h>

//This needs alot of work 

extern float creature_scale_factor;
extern struct structural_innovation_context main_structural_innovation_context;

void position_component_origin(struct creature* creature, struct component* component,
	struct structural_innovation_context* context);

void position_component_at(struct creature* creature, struct component* component, struct component* dest, int position, 
	struct structural_innovation_context* context);

void position_component_random(struct creature* creature, struct component* component, 
	struct structural_innovation_context* context);

void position_component_by_key(struct creature* creature, struct component* component, struct int_sequence key,
	struct structural_innovation_context* context);

struct component* add_component(struct creature* creature, struct component component);

struct creature create_creature(const char name[16]) {
	struct creature new_creature = {
		.inputs = malloc(sizeof(struct component_sequence)),
		.outputs = malloc(sizeof(struct component_sequence))
	};

	strcpy(new_creature.name, name);
	glm_mat4_identity(new_creature.transform);
	vec3 scale = { creature_scale_factor, creature_scale_factor, 1.0f };
	glm_scale(new_creature.transform, scale);

	struct component_sequence in_seq = {
			.capacity = INITIAL_IO_CAPACITY,
			.count = 0,
			.buffer = malloc(sizeof(struct component) * INITIAL_IO_CAPACITY),
			.realloc_amt = INITIAL_IO_CAPACITY
	};

	new_creature.inputs[0] = in_seq;

	struct component_sequence out_seq = {
			.capacity = INITIAL_IO_CAPACITY,
			.count = 0,
			.buffer = malloc(sizeof(struct component) * INITIAL_IO_CAPACITY),
			.realloc_amt = INITIAL_IO_CAPACITY
	};

	new_creature.outputs[0] = out_seq;
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
	for (int i = 0; i < component->io_component.vector_size; i++) {
		component->io_component.ids[i] = ids.buffer[i];
	}
}

void assign_key(struct component* component, struct int_sequence key) {
	component->key = key;
}

void position_component_origin(struct creature* creature, struct component* component,
	struct structural_innovation_context* context) {
	struct int_sequence key = get_origin_key();

	struct add_structural_innovation innovation = get_add_structural_innovation(context,
		component->activity_type, key);
	copy_io_ids(component, innovation.ids);
	assign_color(component, innovation.color);
	assign_key(component, key);
	creature->origin = component;
}


struct component* add_component(struct creature* creature, struct component component) {
	component.this_creature = creature;
	switch (component.io_type) {
	case INPUT:
	{
		struct component* addr = sequence_add_component(creature->inputs, component);
		return addr;
	}
	case OUTPUT:
	{
		struct component* addr = sequence_add_component(creature->outputs, component);
		return addr;
	}
	}
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
			printf("A component was replaced");
		}
	}

	struct add_structural_innovation innovation = get_add_structural_innovation(context,
		component->activity_type, key);
	assign_key(component, key);
	assign_color(component, innovation.color);
	copy_io_ids(component, innovation.ids);
}

void position_component_random(struct creature* creature, struct component* component,
	struct structural_innovation_context* context) {
	int random = rand();
	int current_comps = creature->inputs->count + creature->outputs->count;
	if (current_comps == 0) {
		return;
	}
	int comp_idx = random % current_comps;
	struct component* add_to;
	if (comp_idx < creature->inputs->count) {
		add_to = creature->inputs->buffer + comp_idx;
	}
	else {
		add_to = creature->outputs->buffer + (comp_idx - creature->inputs->count);
	}
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
	(*creature_base) = create_creature("fernando"); 

	struct component origin = create_component(FOOD_SENSOR);
	struct component* origin_addr = add_component(creature_base, origin);
	attach_collider(origin_addr, true);
	position_component_origin(creature_base, origin_addr, &main_structural_innovation_context);

	
	struct component thruster = create_component(THRUSTER);
	struct component* thruster_addr = add_component(creature_base, thruster);
	attach_collider(thruster_addr, true);
	position_component_at(creature_base, thruster_addr, creature_base->origin, 2, &main_structural_innovation_context);
	
	struct component rot = create_component(ROTATOR);
	struct component* rot_addr = add_component(creature_base, rot);
	attach_collider(rot_addr, true);
	position_component_at(creature_base, rot_addr, thruster_addr, 4, &main_structural_innovation_context);
}

void create_simple_creature_2(struct creature* creature_base) {
	(*creature_base) = create_creature("fernando");

	struct component origin = create_component(FOOD_SENSOR);
	struct component* origin_addr = add_component(creature_base, origin);
	attach_collider(origin_addr, true);
	position_component_origin(creature_base, origin_addr, &main_structural_innovation_context);

	struct component thruster = create_component(THRUSTER);
	struct component* thruster_addr = add_component(creature_base, thruster);
	attach_collider(thruster_addr, true);
	position_component_at(creature_base, thruster_addr, creature_base->origin, 2, &main_structural_innovation_context);

	struct component gps = create_component(GPS);
	struct component* gps_addr = add_component(creature_base, gps);
	attach_collider(gps_addr, true);
	position_component_at(creature_base, gps_addr, thruster_addr, 6, &main_structural_innovation_context);
}


void free_creature(struct creature* creature) {
	free(creature->inputs->buffer);
	free(creature->outputs->buffer);
	//free(creature->inputs);
	//free(creature->outputs);
}

bool io_ids_are_equal(struct component* comp1, struct component* comp2) {
	if (comp1->io_component.vector_size == comp2->io_component.vector_size) {
		for (int i = 0; i < comp1->io_component.vector_size; i++) {
			if (comp1->io_component.ids[i] != comp2->io_component.ids[i]) {
				return false;
			}
		}
		return true;
	}
	return false;
}

//this should be sufficient for identifying equivalent comp innovs. New series of ids is returned on new innov
bool io_first_id_is_equal(struct component* comp1, struct component* comp2) { 
	if (comp1->io_component.ids[0] != comp2->io_component.ids[0]) {
		return false;
	}
	return true;
}

void breed_creature_rec(struct component* current_father_component, struct component* current_mother_component, 
	struct creature* dest_creature, bool at_origin);

void breed_creature(struct creature* FATHER_creature, struct creature* mother_creature, struct creature* dest) {
	*dest = create_creature(FATHER_creature->name);
	breed_creature_rec(FATHER_creature->origin, mother_creature->origin,
		dest, true);
}

void breed_creature_rec(struct component* current_father_component, struct component* current_mother_component, struct creature* dest_creature,
	bool at_origin) {
	struct component new_component;
	if (current_father_component == NULL && current_mother_component == NULL) {
		return;
	}else if (current_father_component == NULL) { //disjoint
		new_component = create_component(current_mother_component->activity_type, current_mother_component->color[0],
			current_mother_component->color[1], current_mother_component->color[2]);
	//disjoint || equivalent
	}else if (current_mother_component == NULL || io_first_id_is_equal(current_father_component, current_mother_component)) { 
		new_component = create_component(current_father_component->activity_type, current_father_component->color[0],
			current_father_component->color[1], current_father_component->color[2]);		
	}
	else { //competing
		int random = rand();
		int choice = random % 2;
		if (choice == 0) {
			new_component = create_component(current_father_component->activity_type, current_father_component->color[0],
				current_father_component->color[1], current_father_component->color[2]);
		}
		else {
			new_component = create_component(current_mother_component->activity_type, current_mother_component->color[0],
				current_mother_component->color[1], current_mother_component->color[2]);
		}
	}
	struct component* new_comp_addr = add_component(dest_creature, new_component);
	attach_collider(new_comp_addr, true);
	if (at_origin) {
		position_component_origin(dest_creature, new_comp_addr, &main_structural_innovation_context);
		at_origin = false;
	}
	else {
		position_component_by_key(dest_creature, new_comp_addr,
			current_father_component == NULL ? current_mother_component->key : current_father_component->key,
			&main_structural_innovation_context);
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