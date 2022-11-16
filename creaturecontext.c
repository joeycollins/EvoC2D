#include "creaturecontext.h"
#include "shapes.h"
#include "creature.h"
#include "render.h"
#include "simulation.h"
#include "component.h"
#include "settings.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdbool.h>

extern struct Simulation main_simulation;

void render_creature_indiv(struct creature* creature, unsigned int shader);

void update_creature_context(struct creature_context* context) {
    for (int i = 0; i < context->creatures_count; i++) {
        struct creature* creature = context->creatures[i];
        if (creature->life_stage == ALIVE) {
            update_creature(creature);
            if (main_simulation.draw_objects) {
                render_creature_indiv(creature, context->shader);
            }
        }
    }
}

void render_creature_indiv_rec(struct component* component, mat4 transform, unsigned int shader, float radius, float offset_angle);
//render the creature by drawing each component and joint separately
//could potentially improve performance by looping through component array instead and drawing based on assigned local position, however adding joints
//would limit that. So for now this is the most performative draw method given what the creatures should look like. 
void render_creature_indiv(struct creature* creature, unsigned int shader) {
    use_shader(shader);
    bind_vao(main_simulation.creature_vao.vao);
    unsigned int uniform_loc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)main_simulation.view);
    render_creature_indiv_rec(creature->origin, creature->transform, shader, GROWTH_RADIUS, 0);
}

void render_creature_indiv_rec(struct component* component, mat4 transform, unsigned int shader, float radius, float offset_angle){
    unsigned int uniform_loc = glGetUniformLocation(shader, "transform");
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, transform);
    uniform_loc = glGetUniformLocation(shader, "color");
    glUniform3fv(uniform_loc, 1, component->color);
    glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3));

    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (component->child_exists[i]==true) {
            float angle = (float)(i * GROWTH_ANGLE + offset_angle + GROWTH_ANGLE / 2);
            float sinangle = sinf(angle);
            float cosangle = cosf(angle);
            vec3 joint_offset = { COMPONENT_INRADIUS * cosangle, COMPONENT_INRADIUS * sinangle, 0.0f };
            vec3 comp_offset = { (COMPONENT_RADIUS + radius) * cosangle,  (COMPONENT_RADIUS + radius) * sinangle, 0 };

            mat4 new_transform;
            glm_mat4_copy(transform, new_transform);
            glm_translate(new_transform, comp_offset);

            mat4 joint_transform;
            glm_mat4_copy(transform, joint_transform);
            glm_translate(joint_transform, joint_offset);
            glm_rotate_z(joint_transform, angle - M_PI_2, joint_transform);

            uniform_loc = glGetUniformLocation(shader, "transform");
            glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, joint_transform);
            uniform_loc = glGetUniformLocation(shader, "color");
            glUniform3fv(uniform_loc, 1, component->color);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

            render_creature_indiv_rec(component->children[i], new_transform, shader, radius, (float)(angle + M_PI));
        }
    }
}



struct creature_context create_creature_context(int count, void (*create_creature)(struct creature*), unsigned int shader) {
    struct creature_context context = {
        .shader = shader,
        .creatures_capacity = count,
        .creatures_count = count,
        .creatures = malloc(count * sizeof(struct creature*)),
        .update = &update_creature_context,
        .alive_creatures = 0
    };
    int i = 0;
    struct vao_pool_rendering_info render_info;
    do {
        context.creatures[i] = malloc(sizeof(struct creature));
        create_creature(context.creatures[i]);
        context.alive_creatures += 1;
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