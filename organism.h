#ifndef THE_SURFACE
#define THE_SURFACE

#include "creature.h"
#include "linkednetwork.h"
#include "genome.h"
#include "render.h"
#include "innovation.h"
#include <cglm/mat4.h>

struct organism {
	struct creature creature;
	struct linked_network network;
	struct genome genome;
	struct renderer renderer;
};

void create_culture(int count, void (*create_creature)(struct creature*), struct organism* dest, unsigned int shader_program);

void update_organism(struct organism* organism, float (*activation)(float), mat4 view);

void free_organism(struct organism* organism);
#endif