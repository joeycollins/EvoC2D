#include "component.h"
#include "shapes.h"
#include <stdlib.h>

struct shape create_creature_model(struct creature* creature) {

}

void dfs_build(struct component* root, float* vertices, int* vertices_count, unsigned int* indices, int* indices_count) {

}

struct shape create_component_model(struct component* component, float position_offset[2], float offset_angle) {
	struct shape new_shape = {
		.vertices = malloc(MAX_CHILDREN * 6 * sizeof(float) + component->children_count * 6 * sizeof(float)), //*6 for color data, 1 extra vertex per childd
		.vertices_count = 0,
		.indices = malloc((MAX_CHILDREN - 2) * 3 * sizeof(unsigned int) + component->children_count * 3 * sizeof(unsigned int)),
		.indices_count = 0
	};

	//create base polygon
	for (unsigned int i = 0; i < MAX_CHILDREN; i++) {
		float x = COMPONENT_RADIUS * cos(i * GROWTH_ANGLE + offset_angle) + position_offset[0];
		float y = COMPONENT_RADIUS * sin(i * GROWTH_ANGLE + offset_angle) + position_offset[1];
		
		new_shape.vertices[new_shape.vertices_count] = x;
		new_shape.vertices[new_shape.vertices_count + 1] = y;
		new_shape.vertices[new_shape.vertices_count + 2] = 0.0f; //z index

		for (int j = 0; j < 3; j++) {
			new_shape.vertices[new_shape.vertices_count + 3 + j] = component->color[j]; //add the color data to vertex array
		}

		new_shape.vertices_count += 6;

		if (i < MAX_CHILDREN - 2) {
			new_shape.indices[new_shape.indices_count] = 0;
			new_shape.indices[new_shape.indices_count + 1] = i + 1;
			new_shape.indices[new_shape.indices_count + 2] = i + 2;
			new_shape.indices_count += 3;
		}
	}

	//add connector vertices; small extra overhead for ease of organizing indices
	for (int i = 0; i < component->children_count; i++) {
		float x = GROWTH_RADIUS * cos(i * GROWTH_ANGLE + offset_angle + GROWTH_ANGLE/2) + position_offset[0];
		float y = GROWTH_RADIUS * sin(i * GROWTH_ANGLE + offset_angle + GROWTH_ANGLE/2) + position_offset[1];
		unsigned int true_idx = new_shape.vertices_count / 6;
		new_shape.vertices[new_shape.vertices_count] = x;
		new_shape.vertices[new_shape.vertices_count + 1] = y;
		new_shape.vertices[new_shape.vertices_count + 2] = 0.0f; //z index
		new_shape.vertices[new_shape.vertices_count + 3] = 0.0f; //black connector
		new_shape.vertices[new_shape.vertices_count + 4] = 0.0f;
		new_shape.vertices[new_shape.vertices_count + 5] = 0.0f; 
		new_shape.vertices_count += 6;

		new_shape.indices[new_shape.indices_count] = i;
		new_shape.indices[new_shape.indices_count + 1] = true_idx;
		if (i + 1 < MAX_CHILDREN) {
			new_shape.indices[new_shape.indices_count + 2] = i + 1;
		}
		else {
			new_shape.indices[new_shape.indices_count + 2] = 0;
		}
		new_shape.indices_count += 3;

	}

	return new_shape;
}

void free_shape(struct shape* shape) {
	free(shape->vertices);
	free(shape->indices);
}