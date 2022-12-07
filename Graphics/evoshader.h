#ifndef EVOSHADER_H_INCLUDED
#define EVOSHADER_H_INCLUDED

struct shader_lib {
	unsigned int creature_shader;
	unsigned int food_shader;
	unsigned int text_shader;
	unsigned int inspector_panel_shader;
};

unsigned int build_shader(const char* vertex_shader_path, const char* fragment_shader_path);

#endif // !EVOSHADER_H
