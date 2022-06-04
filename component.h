#ifndef COMPONENT_H
#define COMPONENT_H

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

enum size {
	V2,
	V3
};

enum component_type {
	INPUT,
	OUTPUT
};

union gatherer {
	void (*gather_v2)(vec2*);
	void (*gather_v3)(vec3*);
};

union actualizer {
	void (*actualize_v2)(vec2);
	void (*actualize_v3)(vec3);
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

struct component {
	char name[16];
	float color[3];
	enum component_type io_type;
	union io_component io_component;
};

struct component_sequence {
	unsigned int capacity;
	unsigned int count;
	struct component* buffer;
	unsigned int realloc_amt;
};


#endif