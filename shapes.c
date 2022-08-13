#include "component.h"
#include "shapes.h"
#include <stdlib.h>
#include <math.h>

void dfs_initialize(struct component* root, int* vertices_count, int* indices_count);
void dfs_build(struct shape* shape, struct component* component, float position_offset[2], 
	float offset_angle, float growth_radius, bool decay_growth);


struct shape create_creature_model(struct creature* creature, bool decay_growth) { //decay not implemented
	int vertices = 0, indices = 0;
	dfs_initialize(creature->origin, &vertices, &indices);

	struct shape creature_shape = {
		.vertices = malloc(sizeof(float) * vertices),
		.vertices_count = 0,
		.indices = malloc(sizeof(unsigned int) * indices),
		.indices_count = 0
	};

	float offset[2] = { 0.0f,0.0f };

	dfs_build(&creature_shape, creature->origin, offset, 0, GROWTH_RADIUS, decay_growth);

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
		for (int i = 0; i < MAX_CHILDREN; i++) {
			if (root->child_exists[i]) {
				dfs_initialize(root->children[i], vertices_count, indices_count);
			}
		}
	}
}

float decay_function(float x) {
	return -(float)log(x) + 1;
}

//build the creature from the root component
void dfs_build(struct shape* shape, struct component* component, float position_offset[2], float offset_angle, float growth_radius, bool decay_growth) {
	//Assign the component's local position
	mat4 local_transform;
	glm_mat4_identity(local_transform);
	local_transform[3][0] = position_offset[0];
	local_transform[3][1] = position_offset[1];
	glm_mat4_copy(local_transform, component->local_transform);
	glm_vec2_copy(position_offset, component->local_position);
	//create base polygon
	unsigned int initial_vertex_idx = shape->vertices_count / 6;
	for (int i = 0; i < MAX_CHILDREN; i++) {
		float x = COMPONENT_RADIUS * (float)cos(i * GROWTH_ANGLE + offset_angle) + position_offset[0];
		float y = COMPONENT_RADIUS * (float)sin(i * GROWTH_ANGLE + offset_angle) + position_offset[1];
		
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



	//add connector vertices; small extra overhead for ease of organizing indices (couldve added connectors in the polygon loop)
	for (int i = 0; i < MAX_CHILDREN; i++) {
		if (component->child_exists[i] == false) { continue; }

		float angle = (float)(i * GROWTH_ANGLE + offset_angle + GROWTH_ANGLE / 2);
		float x = (COMPONENT_RADIUS + growth_radius) * (float)cos(angle) + position_offset[0];
		float y = (COMPONENT_RADIUS + growth_radius) * (float)sin(angle) + position_offset[1];

		float offset[2] = { x,y };
		unsigned int next_idx = shape->vertices_count / 6; //index of the first vertex of the next polygon
		dfs_build(shape, component->children[i], offset, (float)(angle + M_PI), growth_radius, decay_growth);

		shape->indices[shape->indices_count] = initial_vertex_idx + i;
		shape->indices[shape->indices_count + 1] = next_idx;
		if (i + 1 < MAX_CHILDREN) {
			shape->indices[shape->indices_count + 2] = initial_vertex_idx + i + 1;
		}
		else {
			shape->indices[shape->indices_count + 2] = initial_vertex_idx;
		}
		shape->indices_count += 3;
	}
}

void free_shape(struct shape* shape) {
	free(shape->vertices);
	free(shape->indices);
}

struct shape create_food_model() {
	struct shape food = {
		.indices_count = 0,
		.vertices_count = 18,
		.vertices = malloc(sizeof(float) * 18)
	};

	float vertices[18] = { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.2f,
							0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.2f,
							0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.2f };

	for (int i = 0; i < 18; i++) {
		food.vertices[i] = vertices[i];
	}

	return food;
}