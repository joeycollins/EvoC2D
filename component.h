#ifndef COMPONENT_H
#define COMPONENT_H

#include "creature.h"
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

#define COMPONENT_RADIUS 0.1f
#define MAX_CHILDREN 7
#define GROWTH_RADIUS 0.50f
#define GROWTH_ANGLE 2.0f * M_PI / MAX_CHILDREN


enum size {
	V1,
	V2,
	V3
};

enum component_type {
	INPUT,
	OUTPUT
};

union gatherer {
	void (*gather_v2)(struct component*, vec2*);
	void (*gather_v3)(struct component*, vec3*);
};

union actualizer {
	void (*actualize_v1)(struct component*, float);
	void (*actualize_v2)(struct component*, vec2);
	void (*actualize_v3)(struct component*, vec3);
};

struct input_component {
	enum size size;
	union gatherer gatherer;
};

struct output_component {
	enum size size;
	union actualizer actualizer;
};

union io_component {
	struct input_component input;
	struct output_component output;
};

struct component_sequence {
	unsigned int capacity;
	unsigned int count;
	struct component* buffer;
	unsigned int realloc_amt;
};

struct component {
	float color[3];
	vec3 local_position;
	enum component_type io_type;
	union io_component io_component;
	int children_count;
	struct component* children[MAX_CHILDREN];
	struct creature* this_creature;
};

struct component create_thruster_component(float r, float g, float b);

struct component create_food_sensor_component(float r, float g, float b);

struct component create_rotator_component(float r, float g, float b);

#endif