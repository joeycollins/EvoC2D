#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include "intsequence.h"
#include "collision.h"
#include "gene.h"
#include "settings.h"
#include <cglm/vec2.h>
#include <cglm/mat4.h>
#include <stdbool.h>

struct creature;

enum component_type {
	INPUT,
	OUTPUT
};

#define ACTIVITY_COUNT 8
enum activity_type {
	THRUSTER,
	ROTATOR,
	FOOD_SENSOR,
	GPS,
	ASEX_REPRO,
	ENERGY_METER,
	CREATURE_SENSOR,
	SEXUAL_REPRO
};

struct io_component {
	int vector_size;
	int* ids;
	void (*activity)(struct component*, float*);
};

struct component_sequence {
	int capacity;
	int count;
	struct component* buffer;
	int realloc_amt;
};


struct component {
	char name[32];
	struct int_sequence key;
	float color[3];
	mat4 local_transform;
	vec2 local_position;
	enum component_type io_type;
	enum activity_type activity_type;
	struct io_component io_component;
	struct gene_sequence genes;
	int children_count;
	struct component* children[MAX_CHILDREN];
	bool child_exists[MAX_CHILDREN];
	struct creature* this_creature;
	struct collider collider;
	float cooldown;
	float cooldown_timer;
};

struct component create_component(enum activity_type activity_type);

void assign_color(struct component* component, float color[3]);

void attach_collider(struct component* component, bool isEnabled);

void get_component_position(struct component* component, vec2* dest);

void free_component(struct component* component);

#endif