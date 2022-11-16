#define _CRT_SECURE_NO_WARNINGS
#include "render.h"
#include "shapes.h"
#include "simulation.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern mat4 view;
extern struct Simulation main_simulation;

void bind_vao(unsigned int vao) {
    if (vao != main_simulation.current_vao) {
        //glBindVertexArray(0);
        glBindVertexArray(vao);
        main_simulation.current_vao = vao;
    }
}

void use_shader(unsigned int shader) {
    if (shader != main_simulation.current_shader) {
        glUseProgram(shader);
        main_simulation.current_shader = shader;
    }
}

unsigned int* get_pool_head(struct vao_pool* vao_pool) {
    return &vao_pool->pool[vao_pool->count - 1].vao;
}

void expand_vao_pool(struct vao_pool* vao_pool) {
    if (vao_pool->count + 1 > vao_pool->pool_capacity) {
        vao_pool->pool = realloc(vao_pool->pool, vao_pool->pool_capacity + 1);
        vao_pool->pool_capacity++;
    }
    unsigned int VBO, EBO, VAO;
    vao_pool->pool[vao_pool->count].current_vbo_offset = 0;
    vao_pool->pool[vao_pool->count].current_ebo_offset = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    bind_vao(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vao_pool->vbo_capacity, NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vao_pool->ebo_capacity, NULL, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    vao_pool->pool[vao_pool->count].vao = VAO;
    vao_pool->pool[vao_pool->count].vbo = VBO;
    vao_pool->pool[vao_pool->count].ebo = EBO;
    vao_pool->count++;
}

struct vao create_individual_draw_mode_vao() {
    unsigned int VBO, EBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    bind_vao(VAO);
    struct shape component_shape = create_indiv_creature_model();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * component_shape.vertices_count, component_shape.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * component_shape.indices_count, component_shape.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    free_shape(&component_shape);

    struct vao res = {
        .vao = VAO,
        .ebo = EBO,
        .vbo = VBO,
    };
    return res;
}

struct vao create_text_vao() {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    bind_vao(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    struct vao new_vao = {
        .vao = VAO,
        .vbo = VBO,
        .current_vbo_offset = 0
    };

    return new_vao;
}

void offset_indices(unsigned int offset, unsigned int* indices, int count) {
    for (int i = 0; i < count; i++) {
        indices[i] += offset;
    }
}

struct vao_pool_rendering_info add_shape_to_pool(struct vao_pool* vao_pool, struct shape shape) {
    int current_vao_idx = vao_pool->count - 1;
    struct vao* current_vao = vao_pool->pool + current_vao_idx;
    if (vao_pool->vbo_capacity - current_vao->current_vbo_offset < shape.vertices_count || vao_pool->ebo_capacity - current_vao->current_ebo_offset < shape.indices_count) {
        expand_vao_pool(vao_pool);
        current_vao = vao_pool->pool + (vao_pool->count - 1);
    }
    bind_vao(current_vao->vao);
    struct vao_pool_rendering_info info = {
        .vao = current_vao->vao,
        .ebo = current_vao->ebo,
        .vbo = current_vao->vbo
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, current_vao->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, current_vao->current_vbo_offset * sizeof(float), sizeof(float) * shape.vertices_count, shape.vertices);
   
    info.vbo_offset = current_vao->current_vbo_offset;
    info.vbo_count = shape.vertices_count;
    offset_indices(current_vao->current_vbo_offset/6, shape.indices, shape.indices_count); //offset ebo indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_vao->ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, current_vao->current_ebo_offset * sizeof(unsigned int), sizeof(unsigned int) * shape.indices_count, shape.indices);

    info.ebo_offset = current_vao->current_ebo_offset;
    info.ebo_count = shape.indices_count;

    //debug subdata
   // unsigned int* ptr = malloc(sizeof(unsigned int) * info.ebo_count);
   // glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, info.ebo_offset * sizeof(unsigned int), info.ebo_count * sizeof(unsigned int), ptr);
   // free(ptr);
    current_vao->current_vbo_offset += shape.vertices_count;
    current_vao->current_ebo_offset += shape.indices_count;
   
    return info;
}

void draw_from_pool_ebo(struct vao_pool_rendering_info info) {
    bind_vao(info.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info.ebo);
    glDrawElements(GL_TRIANGLES, info.ebo_count, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * info.ebo_offset));
}


void draw_from_pool_vbo(struct vao_pool* pool, struct vao_pool_rendering_info info) {
    
}

struct vao_pool create_vao_pool(int ebo_byte_capacity, int vbo_byte_capacity, int pool_capacity) {
    struct vao_pool vp = {
        .count = 0,
        .ebo_capacity = ebo_byte_capacity,
        .vbo_capacity = vbo_byte_capacity,
        .get_head = &get_pool_head,
        .pool = malloc(sizeof(struct vao) * pool_capacity),
        .pool_capacity = pool_capacity
    };

    expand_vao_pool(&vp);

    return vp;
}