#ifndef RENDER_H
#define RENDER_H

#include "shapes.h"
#include <cglm/mat4.h>

struct renderer {
	unsigned int shader;
	unsigned int VAO;
	unsigned int EBO;
	unsigned int VBO;
	int indices_count;
};

void render(mat4* transform, struct renderer* renderer);

struct renderer create_renderer(struct shape* shape, unsigned int shader);

void free_renderer(struct renderer* renderer);
#endif