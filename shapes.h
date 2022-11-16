#ifndef SHAPES_H_INCLUDED
#define SHAPES_H_INCLUDED

#include <stdbool.h>

struct creature;

struct shape {
	float* vertices;
	int vertices_count;
	unsigned int* indices;
	int indices_count;
};

//offset all ebo indices by indices_offset for vao pooling
struct shape create_creature_model(struct creature* creature);

struct shape create_food_model();

struct shape create_inspector_panel_model();

struct shape create_abyss_model();

//for individual draw mode which seems more efficient than pooling. pooling creature models is now deprecated, see render.h
struct shape create_indiv_creature_model();

void free_shape(struct shape* shape);


#endif