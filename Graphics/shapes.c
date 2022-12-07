#include "shapes.h"
#include "creature.h"
#include "component.h"
#include <stdlib.h>
#include <math.h>

void dfs_initialize(struct component* root, int* vertices_count, int* indices_count);
void dfs_build(struct shape* shape, struct component* component, float position_offset[2], 
	float offset_angle, float growth_radius);

//deprecated, see create_individ_creature model for new rendering method
struct shape create_creature_model(struct creature* creature) { //decay not implemented
	int vertices = 0, indices = 0;
	dfs_initialize(creature->origin, &vertices, &indices);

	struct shape creature_shape = {
		.vertices = malloc(sizeof(float) * vertices),
		.vertices_count = 0,
		.indices = malloc(sizeof(unsigned int) * indices),
		.indices_count = 0
	};

	float offset[2] = { 0.0f,0.0f };

	dfs_build(&creature_shape, creature->origin, offset, 0, GROWTH_RADIUS);

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
void dfs_build(struct shape* shape, struct component* component, float position_offset[2], float offset_angle, float growth_radius) {
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
		dfs_build(shape, component->children[i], offset, (float)(angle + M_PI), growth_radius);

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
		.indices_count = 3,
		.vertices_count = 18,
		.vertices = malloc(sizeof(float) * 18),
		.indices = malloc(sizeof(unsigned int) * 3)
	};

	float vertices[18] = { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.2f,
							0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.2f,
							0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.2f };


	for (int i = 0; i < 18; i++) {
		food.vertices[i] = vertices[i];
	}

	for (int i = 0; i < 3; i++) {
		food.indices[i] = i;
	}

	return food;
}

struct shape create_inspector_panel_model() {
	struct shape panel = {
		.vertices_count = 24,
		.indices_count = 6,
		.indices = malloc(sizeof(unsigned int) * 6),
		.vertices = malloc(sizeof(float) * 24)
	};

	float vertices[24] = {
		0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
		0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
		0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
		0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
	};

	for (int i = 0; i < 24; i++) {
		panel.vertices[i] = vertices[i];
	}

	unsigned int indices[6] = {
		0, 1, 2, 0, 2, 3
	};

	for (int i = 0; i < 6; i++) {
		panel.indices[i] = indices[i];
	}

	return panel;

}

struct shape create_abyss_model() {

	int sides = 30;
	float radius = 1;
	float angle = 2.0f * M_PI / sides;
	
	struct shape new_shape = {
		.vertices_count = 0,
		.indices_count = 0,
		.indices = malloc(sizeof(unsigned int) * (sides - 2) * 3),
		.vertices = malloc((sizeof(float) * sides * 6))
	};

	for (int i = 0; i < sides; i++) {
		float x = radius * (float)cos(i * angle);
		float y = radius * (float)sin(i * angle);

		new_shape.vertices[new_shape.vertices_count] = x;
		new_shape.vertices[new_shape.vertices_count + 1] = y;
		new_shape.vertices[new_shape.vertices_count + 2] = 0.0f; //z index

		new_shape.vertices[new_shape.vertices_count + 3] = 0.29f;//color
		new_shape.vertices[new_shape.vertices_count + 4] = 0;
		new_shape.vertices[new_shape.vertices_count + 5] = 0.5f;

		new_shape.vertices_count += 6;

		if (i < sides - 2) {
			new_shape.indices[new_shape.indices_count] = 0;
			new_shape.indices[new_shape.indices_count + 1] = i + 1;
			new_shape.indices[new_shape.indices_count + 2] = i + 2;
			new_shape.indices_count += 3;
		}
	}

	return new_shape;
}

struct shape create_indiv_creature_model() {
	float side = 2* COMPONENT_RADIUS * sinf(M_PI / MAX_CHILDREN);
	struct shape res = {
		.indices_count = 3,
		.vertices_count = 9,
		.indices = malloc(sizeof(unsigned int) * ((MAX_CHILDREN - 2) * 3 + 3)),
		.vertices = malloc(sizeof(float) * (MAX_CHILDREN * 3 + 9))
	};

	res.vertices[0] = -side / 2.0f;
	res.vertices[1] = 0;
	res.vertices[2] = 0;

	res.vertices[3] = side / 2.0f;
	res.vertices[4] = 0;
	res.vertices[5] = 0;

	res.vertices[6] = 0;
	res.vertices[7] = GROWTH_RADIUS;
	res.vertices[8] = 0;

	res.indices[0] = 0;
	res.indices[1] = 1;
	res.indices[2] = 2;

	unsigned int indices_offset = 3;
	for (int i = 0; i < MAX_CHILDREN; i++) {
		float x = COMPONENT_RADIUS * (float)cos(i * GROWTH_ANGLE);
		float y = COMPONENT_RADIUS * (float)sin(i * GROWTH_ANGLE);

		res.vertices[res.vertices_count] = x;
		res.vertices[res.vertices_count + 1] = y;
		res.vertices[res.vertices_count + 2] = 0.0f; //z index

		res.vertices_count += 3;

		if (i < MAX_CHILDREN - 2) {
			res.indices[res.indices_count] = indices_offset;
			res.indices[res.indices_count + 1] = indices_offset+ i + 1;
			res.indices[res.indices_count + 2] = indices_offset+ i + 2;
			res.indices_count += 3;
		}
	}

	return res;
}
