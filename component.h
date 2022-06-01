#ifndef COMPONENT_H
#define COMPONENT_H

#include "creature.h" 
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

struct component {
	char name[16];
	unsigned int size;
};
struct input_component_v2 {
	struct component base;
	void (*gather)(vec2*);
};

struct input_component_v3 {
	struct component base;
	void (*gather)(vec3*);
};

struct output_component_v2 {
	struct component base;
	void (*actualize)(vec2*);
};

struct output_component_v3 {
	struct component base;
	void (*actualize)(vec3*);
};

#endif