#include "organism.h"
#include "shapes.h"
#include "component.h"
#include "food.h"

extern struct innovation_context main_innovation_context;
extern struct food_context main_food_context;
extern float creature_scale_factor;
extern float food_scale_factor;
extern float delta_time;
extern mat4 view;

void breed(struct organism* FATHER, struct organism* mother, struct organism* dest, unsigned int shader);

void create_organism(struct organism* dest, void (*create_creature)(struct creature*), unsigned int shader_program);

struct culture create_culture(int count, void (*create_creature)(struct creature*), unsigned int shader_program) {
	struct culture this_culture = {
		.count = count,
		.alive_count = count,
		.population = malloc(sizeof(struct organism) * count)
	};
	for (int i = 0; i < count; i++) {
		
		if (i % 1 == 0) {
			create_organism(this_culture.population + i, create_creature, shader_program);
		}
		else {
			create_organism(this_culture.population + i, &create_simple_creature_2, shader_program);
		}
	}

	return this_culture;
}

void evaluate_collision(struct organism* organism) {
	int inputs_count = organism->creature.inputs->count;
	int outputs_count = organism->creature.outputs->count;
	for (int i = 0; i < inputs_count + outputs_count; i++) {
		struct component* component;
		if (i < inputs_count) {
			component = &organism->creature.inputs->buffer[i];
		}
		else {
			component = &organism->creature.outputs->buffer[i - inputs_count];
		}
		if (component->collider.enabled) {
			vec2 pos;
			get_component_position(component, pos);
			for (int j = 0; j < main_food_context.food_count; j++) {
				if (!main_food_context.food[j].alive) { continue; }
				bool is_coll = component->collider.collision_fn(pos[0], pos[1], COMPONENT_RADIUS * creature_scale_factor,
					main_food_context.food[j].transform[3][0], main_food_context.food[j].transform[3][1], 50.0f);
				if (is_coll) {
					organism->remaining_life_span += 5.0f;
					main_food_context.food[j].alive = false;
				}
			}
		}
	}
}

void evaluate_organism(struct organism* organism, float (*activation)(float)) {
	evaluate_linked_network(&organism->network, activation);
}

void render_organism(struct organism* organism) {
	render(&organism->creature.transform, &organism->renderer);
}

//returns whether the organism is alive
bool update_organism(struct organism* organism, float (*activation)(float)) {
	evaluate_organism(organism, activation);
	evaluate_collision(organism);
	render_organism(organism, view);
	organism->remaining_life_span -= delta_time;
	if (organism->remaining_life_span < 0.0f) {
		organism->is_alive = false;
		return false;
	}
	return true;
}

void free_organism(struct organism* organism) {
	free_linked_network(&organism->network);
	free_genome(&organism->genome);
	free_creature(&organism->creature);	
	free_renderer(&organism->renderer);
}

void create_organism(struct organism* dest, void (*create_creature)(struct creature*), unsigned int shader_program) {
	create_creature(&dest->creature);
	struct shape creature_shape = create_creature_model(&dest->creature, false);
	dest->genome = create_genome(&dest->creature);
	initial_mutate(&dest->genome, &main_innovation_context);
	dest->network = create_linked_network(&dest->genome);
	dest->renderer = create_renderer(&creature_shape, shader_program);
	free_shape(&creature_shape);
	dest->is_alive = true;
	dest->life_span = INITIAL_LIFE_SPAN;
	dest->remaining_life_span = INITIAL_LIFE_SPAN;
}

void breed(struct organism* FATHER, struct organism* mother, struct organism* dest, unsigned int shader) {
	breed_creature(&FATHER->creature, &mother->creature, &dest->creature);
	struct shape creature_shape = create_creature_model(&dest->creature, false);
	dest->genome = create_genome(&dest->creature);
	breed_genomes(&FATHER->genome, &mother->genome, &dest->genome);
	mutate(&dest->genome, &main_innovation_context,
		0.5f, 0.5f, 0.35f);
	dest->network = create_linked_network(&dest->genome);
	dest->renderer = create_renderer(&creature_shape, shader);
	free_shape(&creature_shape);
	dest->is_alive = true;
	dest->life_span = INITIAL_LIFE_SPAN;
	dest->remaining_life_span = INITIAL_LIFE_SPAN;
}

void update_culture(struct culture* culture, float (*activation)(float)) {
	for (int i = 0; i < culture->count; i++) {
		struct organism* this_organism = culture->population + i;
		if (this_organism->is_alive) {
			bool alive = update_organism(this_organism, activation);
			if (!alive) {
				culture->alive_count--;
				if (culture->alive_count == 1) {
					culture->elder_beta = this_organism;
				}
				else if (culture->alive_count == 0) {
					culture->elder = this_organism;
				}
			}
		}
	}
}

struct culture create_culture_from_best(struct culture* culture, int count, unsigned int shader_program) {
	struct culture this_culture = {
		.count = count,
		.alive_count = count,
		.population = malloc(sizeof(struct organism) * count)
	};
	for (int i = 0; i < count; i++) {
		breed(culture->elder, culture->elder_beta, this_culture.population + i, shader_program);
	}
	
	return this_culture;
}

void free_culture(struct culture* culture) {
	for (int i = 0; i < culture->count; i++) {
		free_organism(culture->population + i);
	}
	free(culture->population);
}
