#include "abysscontext.h"
#include "shapes.h"
#include "simulation.h"
#include "utils.h"
#include "settings.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

void update_abyss_context(struct abyss_context* context, mat4 view){
	for (int i = 0; i < context->abysses_count; i++) {
		struct abyss* this_abyss = context->abysses + i;
		
		float new_angle = this_abyss->angle + main_simulation.delta_time;
		vec3 translation = { cosf(new_angle) * ABYSS_TRAVEL_RADIUS, sinf(new_angle) * ABYSS_TRAVEL_RADIUS, 0 };
		this_abyss->transform[3][0] = translation[0];
		this_abyss->transform[3][1] = translation[1];
		this_abyss->angle = new_angle;
		unsigned int shader = context->shader;
		use_shader(shader);
		unsigned int uniform_loc = glGetUniformLocation(shader, "transform");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)this_abyss->transform);
		uniform_loc = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)view);
		draw_from_pool_ebo(context->rendering_info);
	}
}

struct abyss_context create_abyss_context(int count, unsigned int shader_program) {
	struct abyss_context context = {
		.abysses_count = count,
		.abysses = malloc(sizeof(struct abyss) * count),
		.update = &update_abyss_context,
		.shader = shader_program,
	};

	struct shape ashape = create_abyss_model();
	context.rendering_info = add_shape_to_pool(&main_simulation.vao_pool, ashape);
	free_shape(&ashape);

	for (int i = 0; i < count; i++) {
		//get_random_translation(0, 0, CREATURE_SPAWN_RADIUS, context.abysses[i].transform);
		context.abysses[i].angle = 0;
		vec3 transl = { cos(0) * ABYSS_TRAVEL_RADIUS, sin(0) * ABYSS_TRAVEL_RADIUS, 0 };
		vec3 scale = { 400, 400, 1.0f };
		glm_mat4_identity(context.abysses[i].transform);
		glm_translate(context.abysses[i].transform, transl);
		glm_scale(context.abysses[i].transform, scale);

	}

	return context;
}