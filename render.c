#define _CRT_SECURE_NO_WARNINGS
#include "render.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void render(mat4* transform, struct renderer* renderer) {
    glUseProgram(renderer->shader);
    unsigned int transform_loc = glGetUniformLocation(renderer->shader, "transform");
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transform);
    glBindVertexArray(renderer->VAO);
    if (renderer->indices_count > 0) {
        glDrawElements(GL_TRIANGLES, renderer->indices_count, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glBindVertexArray(0);
}

//create a renderer to fit with colored vertex shader
struct renderer create_renderer(struct shape* shape, unsigned int shader) {
	struct renderer this_renderer = {
		.shader = shader,
        .indices_count = shape->indices_count
	};

    glGenVertexArrays(1, &this_renderer.VAO);
    glGenBuffers(1, &this_renderer.VBO);
    if (shape->indices_count > 0) { //Dont need ebo if there are 0 indices
        glGenBuffers(1, &this_renderer.EBO);
    }

    glBindVertexArray(this_renderer.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this_renderer.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shape->vertices_count, shape->vertices, GL_STATIC_DRAW);

    if (shape->indices_count > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this_renderer.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shape->indices_count, shape->indices, GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    return this_renderer;
}

void delete_renderer(struct renderer* renderer) {
    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteBuffers(1, &renderer->VBO);
}