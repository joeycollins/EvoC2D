#ifndef RENDER_H
#define RENDER_H

#include "shapes.h"

struct renderer {
	unsigned int shader;
	unsigned int VAO;
	unsigned int EBO;
	unsigned int VBO;
	int indices_count;
	void (*draw)(struct renderer*);
};

struct renderer create_creature_renderer(struct shape* shape, unsigned int shader);

void delete_renderer(struct renderer* renderer);
#endif