#include "component.h"
#include "shapes.h"
#include <stdlib.h>

void dfs_initialize(struct component* root, int* vertices_count, int* indices_count);
void dfs_build(struct shape* shape, struct component* component, float position_offset[2], float offset_angle);

struct shape create_creature_model(struct creature* creature) {
	int vertices = 0, indices = 0;
	dfs_initialize(creature->origin, &vertices, &indices);

	struct shape creature_shape = {
		.vertices = malloc(sizeof(float) * vertices),
		.vertices_count = 0,
		.indices = malloc(sizeof(unsigned int) * indices),
		.indices_count = 0
	};

	float offset[2] = { 0.0f,0.0f };

	dfs_build(&creature_shape, creature->origin, offset, 0);

	return creature_shape;

}

//count the amount of vertices and indices to allocate for
void dfs_initialize(struct component* root, int* vertices_count, int* indices_count) {
	if (root == NULL) {
		return;
	}
	else {
		*vertices_count += MAX_CHILDREN * 6;
		*indices_count += (MAX_CHILDREN - 2) * 3 + root->children_count * 3;
		for (int i = 0; i < root->children_count; i++) {
			dfs_initialize(root->children[i], vertices_count, indices_count);
		}
	}
}

//build the creature from the root component
void dfs_build(struct shape* shape, struct component* component, float position_offset[2], float offset_angle) {
	//create base polygon
	unsigned int initial_vertex_idx = shape->vertices_count / 6;
	for (unsigned int i = 0; i < MAX_CHILDREN; i++) {
		float x = COMPONENT_RADIUS * cos(i * GROWTH_ANGLE + offset_angle) + position_offset[0];
		float y = COMPONENT_RADIUS * sin(i * GROWTH_ANGLE + offset_angle) + position_offset[1];
		
		shape->vertices[shape->vertices_count] = x;
		shape->vertices[shape->vertices_count + 1] = y;
		shape->vertices[shape->vertices_count + 2] = 0.0f; //z index

		for (int j = 0; j < 3; j++) {
			shape->vertices[shape->vertices_count + 3 + j] = component->color[j]; //add the color data to vertex array
		}

		shape->vertices_count += 6;

		if (i < MAX_CHILDREN - 2) {
			shape->indices[shape->indices_count] = initial_vertex_idx;
			shape->indices[shape->indices_count + 1] = initial_vertex_idx + i + 1;
			shape->indices[shape->indices_count + 2] = initial_vertex_idx + i + 2;
			shape->indices_count += 3;
		}
	}

	//add connector vertices; small extra overhead for ease of organizing indices
	for (int i = 0; i < component->children_count; i++) {
		float angle = i * GROWTH_ANGLE + offset_angle + GROWTH_ANGLE / 2;
		float x = (COMPONENT_RADIUS + GROWTH_RADIUS) * cos(angle) + position_offset[0];
		float y = (COMPONENT_RADIUS + GROWTH_RADIUS) * sin(angle) + position_offset[1];

		float offset[2] = { x,y };
		unsigned int true_idx = shape->vertices_count / 6;
		dfs_build(shape, component->children[i], offset, angle + M_PI);

		shape->indices[shape->indices_count] = i;
		shape->indices[shape->indices_count + 1] = true_idx;
		if (i + 1 < MAX_CHILDREN) {
			shape->indices[shape->indices_count + 2] = i + 1;
		}
		else {
			shape->indices[shape->indices_count + 2] = 0;
		}
		shape->indices_count += 3;

	}
}

void free_shape(struct shape* shape) {
	free(shape->vertices);
	free(shape->indices);
}