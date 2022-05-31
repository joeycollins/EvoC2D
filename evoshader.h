#ifndef EVOSHADER_H
#define EVOSHADER_H
#include <stdbool.h>

unsigned int build_shader(const char* vertex_shader_path, const char* fragment_shader_path);

void use_shader(unsigned int id);

void set_uniform_bool(unsigned int id, const char* uniform_name, bool value);

void set_uniform_int(unsigned int id, const char* uniform_name, int value);

void set_uniform_float(unsigned int id, const char* uniform_name, float value);

#endif // !EVOSHADER_H
