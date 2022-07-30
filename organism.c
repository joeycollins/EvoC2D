#include "organism.h"
#include "shapes.h"
#include "component.h"
#include "food.h"

extern struct innovation_context main_innovation_context;
extern struct food_context main_food_context;
extern float creature_scale_factor;
extern float food_scale_factor;
extern mat4 view;

void create_culture(int count, void (*create_creature)(struct creature*), struct organism* dest, unsigned int shader_program) {
	for (int i = 0; i < count; i++) {
		
		create_creature(&dest[i].creature);
		dest[i].genome = create_genome(&dest[i].creature);
		mutate_add_connection(&dest[i].genome, &main_innovation_context);
		mutate_add_connection(&dest[i].genome, &main_innovation_context);
		mutate_add_gene(&dest[i].genome, &main_innovation_context);
		mutate_add_connection(&dest[i].genome, &main_innovation_context);
		mutate_add_gene(&dest[i].genome, &main_innovation_context);
		mutate_add_gene(&dest[i].genome, &main_innovation_context);
		mutate_add_connection(&dest[i].genome, &main_innovation_context);
		mutate_add_connection(&dest[i].genome, &main_innovation_context);
		dest[i].network = create_linked_network(&dest[i].genome);
		struct shape org_shape = create_creature_model(&dest[i].creature, false);
		dest[i].renderer = create_renderer(&org_shape, shader_program);
		free_shape(&org_shape);
	}
}

void evaluate_collision(struct organism* organism) {
	for (int i = 0; i < organism->creature.inputs->count; i++) {
		struct component* component = &organism->creature.inputs->buffer[i];
		if (component->collider.enabled) {
			vec2 pos;
			get_component_position(component, pos);
			for (int j = 0; j < main_food_context.food_count; j++) {
				if (!main_food_context.food[j].alive) { continue; }
				bool is_coll = component->collider.collision_fn(pos[0], pos[1], COMPONENT_RADIUS * creature_scale_factor,
					main_food_context.food[j].transform[3][0], main_food_context.food[j].transform[3][1], 50.0f);
				if (is_coll) {
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

void update_organism(struct organism* organism, float (*activation)(float)) {
	evaluate_organism(organism, activation);
	evaluate_collision(organism);
	render_organism(organism, view);
}

void free_organism(struct organism* organism) {
	free_linked_network(&organism->network);
	free_genome(&organism->genome);
	free_creature(&organism->creature);	
	free_renderer(&organism->renderer);
}