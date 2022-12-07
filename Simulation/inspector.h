#ifndef INSPECTOR_H_INCLUDED
#define INSPECTOR_H_INCLUDED

#include "render.h"
#include <stdbool.h>
#include <cglm/mat4.h>

struct creature;

struct inspector {
	bool enabled;
	struct creature* target_creature;
	struct vao_pool_rendering_info rendering_info; //panel is going to be in the pool
	void (*update)(struct inspector*);
	mat4 panel_transform;
	unsigned int panel_shader;
	unsigned int text_shader;
};


struct inspector create_inspector(unsigned int panel_shader_program, unsigned int text_shader_program);

void update_inspector_target(struct inspector* inspector, struct creature* creature);

#endif