#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <cglm/mat4.h>

struct shape;

struct vao {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	int current_vbo_offset;
	int current_ebo_offset;
	int capacity;
};

struct vao_pool {
	int count;
	int pool_capacity;
	struct vao* pool;
	unsigned int (*get_head)(struct vao_pool* vao_pool);
	int ebo_capacity;
	int vbo_capacity;
};

struct vao_pool_rendering_info {
	int vbo_offset;
	int ebo_offset;
	int vbo_count;
	int ebo_count;
	unsigned int vao;
	unsigned int ebo;
	unsigned int vbo;
};

void bind_vao(unsigned int vao);

void use_shader(unsigned int shader);

//POOLING DEPRECATED FOR CREATURE MODELS.
//Keep code for now incase I want to use in the future
void expand_vao_pool(struct vao_pool* pool);

struct vao_pool create_vao_pool(int ebo_byte_capacity, int vbo_byte_capacity, int pool_capacity);

struct vao_pool_rendering_info add_shape_to_pool(struct vao_pool* vao_pool, struct shape shape);

void draw_from_pool_ebo(struct vao_pool_rendering_info info);

void draw_from_pool_vbo(struct vao_pool* pool, struct vao_pool_rendering_info info);


//Preferred rendering method, create individual vaos
//Must refine so that the vao object contains subarray pointers, for now explicitly setting offsets in render functions is fine

struct vao create_individual_draw_mode_vao();

struct vao create_text_vao();
#endif