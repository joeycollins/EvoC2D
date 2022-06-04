#ifndef SHAPES_H
#define SHAPES_H

const float square_vertices[] = {
	0.5f,  0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f
};

const unsigned int square_indices[] = {
	0, 1, 3,
	1, 2, 3
};

const float eq_triangle_vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};



#endif