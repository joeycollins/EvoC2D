#include "creaturecontext.h"
#include "shapes.h"
#include "creature.h"
#include "render.h"
#include "simulation.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/mat4.h>

extern struct Simulation main_simulation;

void render_creature(struct creature* creature, struct creature_context* context);

void update_creature_context(struct creature_context* context) {
    for (int i = 0; i < context->creatures_count; i++) {
        struct creature* creature = context->creatures[i];
        if (creature->life_stage == ALIVE) {
            update_creature(creature);
            render_creature(creature, context);
        }
    }
}

void render_creature(struct creature* creature, struct creature_context* context) {
    unsigned int shader = context->shader;
    use_shader(shader);
    unsigned int uniform_loc = glGetUniformLocation(shader, "transform");
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)creature->transform);
    uniform_loc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)main_simulation.view);
    draw_from_pool_ebo(creature->rendering_info);
}


struct creature_context create_creature_context(int count, void (*create_creature)(struct creature*), unsigned int shader) {
    struct creature_context context = {
        .shader = shader,
        .creatures_capacity = count,
        .creatures_count = count,
        .creatures = malloc(count * sizeof(struct creature*)),
        .update = &update_creature_context
    };
    int i = 0;
    struct vao_pool_rendering_info render_info;
    do {
        context.creatures[i] = malloc(sizeof(struct creature));
        create_creature(context.creatures[i]);
        if (i == 0) {
            struct shape creature_shape = create_creature_model(context.creatures[i]);
            render_info = add_shape_to_pool(&main_simulation.vao_pool, creature_shape);
            free_shape(&creature_shape);
        }
        context.creatures[i]->rendering_info = render_info;
        i++;
    } while (i < count);
    return context;
}

struct creature* add_to_context(struct creature_context* context) {
    for (int i = 0; i < context->creatures_count; i++) {
        struct creature* creature = context->creatures[i];
        if (creature->life_stage != ALIVE) {
            free_creature(creature);
            return creature;
        }
    }
    struct creature* creature = malloc(sizeof(struct creature));
    if (context->creatures_count == context->creatures_capacity) {
        context->creatures_capacity += 500;
        context->creatures = realloc(context->creatures,context->creatures_capacity * sizeof(struct creature*));
    }
    context->creatures[context->creatures_count] = creature;
    context->creatures_count++;
    return creature;
}