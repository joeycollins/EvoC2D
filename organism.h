#ifndef THE_SURFACE
#define THE_SURFACE

#include "creature.h"
#include "linkednetwork.h"
#include "genome.h"
#include "render.h"
#include "innovation.h"
#include <cglm/mat4.h>
#include <stdbool.h>

#define INITIAL_LIFE_SPAN 15.0f

struct organism {
    float life_span;
	float remaining_life_span;
	bool is_alive;
	struct creature creature;
	struct linked_network network;
	struct genome genome;
	struct renderer renderer;
};

struct culture {
	int count;
	int alive_count;
	struct organism* elder;
	struct organism* elder_beta;
	struct organism* population;
};

struct culture create_culture(int count, void (*create_creature)(struct creature*), unsigned int shader_program);

bool update_organism(struct organism* organism, float (*activation)(float));

void free_organism(struct organism* organism);

void update_culture(struct culture* culture, float (*activation)(float));

struct culture create_culture_from_best(struct culture* culture, int count, unsigned int shader_program);

void free_culture(struct culture* culture);
#endif