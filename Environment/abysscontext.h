#ifndef ABYSS_CONTEXT_H_INCLUDED
#define ABYSS_CONTEXT_H_INCLUDED

#include "render.h"
#include <cglm/mat4.h>

#define ABYSS_TRAVEL_RADIUS 1000

struct abyss {
	mat4 transform;
	float angle;
};

struct abyss_context {
	int abysses_count;
	struct abyss* abysses;
	struct vao_pool_rendering_info rendering_info;
	void (*update)(struct abyss_context*, mat4);
	unsigned int shader;
};

struct abyss_context create_abyss_context(int count, unsigned int shader_program);

#endif