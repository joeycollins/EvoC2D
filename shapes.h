#ifndef SHAPES_H
#define SHAPES_H

struct shape {
	float* vertices;
	int vertices_count;
	unsigned int* indices;
	int indices_count;
};

struct shape create_creature_model(struct creature* creature);

struct shape create_component_model(struct component* component, float position_offset[2], float offset_angle);

void free_shape(struct shape* shape);

#endif