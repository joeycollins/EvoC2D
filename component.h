#ifndef COMPONENT_H
#define COMPONENT_H

#include "intsequence.h"
#include "collision.h"
#include "creature.h"
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <stdbool.h>

#define COMPONENT_RADIUS 0.1f
#define MAX_CHILDREN 7
#define GROWTH_RADIUS 0.50f
#define GROWTH_ANGLE 2.0f * M_PI / MAX_CHILDREN

enum component_type {
	INPUT,
	OUTPUT
};

enum activity_type {
	THRUSTER,
	ROTATOR,
	FOOD_SENSOR,
	GPS
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
	struct int_sequence key;
	float color[3];
	mat4 local_transform;
	vec2 local_position;
	enum component_type io_type;
	enum activity_type activity_type;
	struct io_component io_component;
	int children_count;
	struct component* children[MAX_CHILDREN];
	bool child_exists[MAX_CHILDREN];
	struct creature* this_creature;
	struct collider collider;
};

struct component create_thruster_component();

struct component create_food_sensor_component();

struct component create_rotator_component();

struct component create_component(enum activity_type activity_type);

void assign_color(struct component* component, float color[3]);

void attach_collider(struct component* component, bool isEnabled);

void get_component_position(struct component* component, vec2* dest);

#endif