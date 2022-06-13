#define _CRT_SECURE_NO_WARNINGS
#include "render.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>


void render_creature(struct renderer* renderer) {
    glUseProgram(renderer->shader);
    mat4 trans;
    glm_mat4_identity(trans);
    unsigned int transform_loc = glGetUniformLocation(renderer->shader, "transform");
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, trans);
    glBindVertexArray(renderer->VAO);
    glDrawElements(GL_TRIANGLES, renderer->indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

struct renderer create_creature_renderer(struct shape* shape, unsigned int shader) {
	struct renderer this_renderer = {
		.shader = shader,
        .indices_count = shape->indices_count
	};

    glGenVertexArrays(1, &this_renderer.VAO);
    glGenBuffers(1, &this_renderer.VBO);
    glGenBuffers(1, &this_renderer.EBO);

    glBindVertexArray(this_renderer.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this_renderer.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shape->vertices_count, shape->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this_renderer.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shape->indices_count, shape->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    this_renderer.draw = &render_creature;
    
    return this_renderer;
}

void delete_renderer(struct renderer* renderer) {
    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteBuffers(1, &renderer->VBO);
}