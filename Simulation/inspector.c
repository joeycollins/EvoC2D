#include "inspector.h"
#include "creature.h"
#include "render.h"
#include "simulation.h"
#include "text.h"
#include "utils.h"
#include "component.h"
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern struct Simulation main_simulation;

void render_panel(struct inspector* inspector, float x_offset, float vertical_scaling) {
	unsigned int shader = inspector->panel_shader;
	use_shader(shader);
	unsigned int uniform_loc = glGetUniformLocation(shader, "offset");
	vec2 offset = { x_offset,30};
	glUniform2fv(uniform_loc, 1, offset);
	uniform_loc = glGetUniformLocation(shader, "scaling");
	vec2 scaling = { main_simulation.screen_width / 2 , vertical_scaling};
	glUniform2fv(uniform_loc, 1, scaling);
	draw_from_pool_ebo(inspector->rendering_info);
	
}

void update_inspector(struct inspector* inspector) {
	if (inspector->enabled && inspector->target_creature != NULL) {
		float x_offset = main_simulation.screen_width - (main_simulation.screen_width / 4) - 30;
		float vertical_scaling = main_simulation.screen_height - 60;
		float text_x_offset = 30, text_y_offset = 60;
		float text_x_offset2 = 450;
		render_panel(inspector,x_offset, vertical_scaling);
		float text_color[3] = { 0,0,0 };
		float red_color[3] = { 1, 0, 0 };
		float green_color[3] = { 0, 1,0 };
		render_text(inspector->text_shader, "Inspector", x_offset + text_x_offset, vertical_scaling - text_y_offset, 2.0f, text_color);
		render_text(inspector->text_shader, "Name: ", x_offset + text_x_offset, vertical_scaling - text_y_offset*2 , 1.0f, text_color);
		render_text(inspector->text_shader, inspector->target_creature->name, x_offset + text_x_offset + text_x_offset2, vertical_scaling - text_y_offset * 2, 1.0f, text_color);
		render_text(inspector->text_shader, "Generation: ", x_offset + text_x_offset, vertical_scaling - text_y_offset*3, 1.0f, text_color);
		char* gen_str=int_to_string(inspector->target_creature->generation);
		render_text(inspector->text_shader, gen_str, x_offset + text_x_offset + text_x_offset2, vertical_scaling - text_y_offset * 3, 1.0f, text_color);
		free(gen_str);

		render_text(inspector->text_shader, "Remaining Life: ", x_offset + text_x_offset, vertical_scaling - text_y_offset * 4, 1.0f, text_color);

		char* life_str = float_to_string(inspector->target_creature->remaining_life_span);
		render_text(inspector->text_shader, life_str, x_offset + text_x_offset + text_x_offset2, vertical_scaling - text_y_offset * 4, 1.0f, text_color);
		free(life_str);

		render_text(inspector->text_shader, "State: ", x_offset + text_x_offset, vertical_scaling - text_y_offset * 5, 1.0f, text_color);


		switch (inspector->target_creature->life_stage) {
		case ALIVE:
			render_text(inspector->text_shader, "Alive", x_offset + text_x_offset + text_x_offset2, vertical_scaling - text_y_offset * 5, 1.0f, green_color);
			break;
		default:
			render_text(inspector->text_shader, "Dead", x_offset + text_x_offset + text_x_offset2, vertical_scaling - text_y_offset * 5, 1.0f, red_color);
			break;
		}

		render_text(inspector->text_shader, "Components: ", x_offset + text_x_offset, vertical_scaling - text_y_offset * 7, 2.0f, text_color);

		for (int i = 0; i < inspector->target_creature->components.count; i++) {
			struct component* component = inspector->target_creature->components.buffer + i;
			render_text(inspector->text_shader, component->name, x_offset + text_x_offset, vertical_scaling - text_y_offset * (9 + i), 1.0f, component->color);
		}

	}
}

struct inspector create_inspector(unsigned int panel_shader_program, unsigned int text_shader_program) {
	struct inspector new_inspector = {
		.enabled = true,
		.target_creature = NULL,
		.update = &update_inspector,
		.panel_shader = panel_shader_program,
		.text_shader = text_shader_program
	};
	//add to vao pool 
	struct shape panel_shape = create_inspector_panel_model();
	new_inspector.rendering_info = add_shape_to_pool(&main_simulation.vao_pool, panel_shape);
	free_shape(&panel_shape);


	return new_inspector;
}

void update_inspector_target(struct inspector* inspector, struct creature* creature) {
	inspector->target_creature = creature;
}

