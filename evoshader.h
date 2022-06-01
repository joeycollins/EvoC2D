#ifndef EVOSHADER_H
#define EVOSHADER_H
#include <stdbool.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>
#include <cglm/mat2.h>
#include <cglm/mat3.h>
#include <cglm/mat4.h>

unsigned int build_shader(const char* vertex_shader_path, const char* fragment_shader_path);

void use_shader(unsigned int id);

void set_uniform_bool(unsigned int id, const char* uniform_name, bool value);

void set_uniform_int(unsigned int id, const char* uniform_name, int value);

void set_uniform_float(unsigned int id, const char* uniform_name, float value);

void set_vec2(unsigned int id, const char* uniform_name, const vec2* value);

void set_vec2_f(unsigned int id, const char* uniform_name, float x, float y);

void set_vec3(unsigned int id, const char* uniform_name, const vec3* value);

void set_vec3_f(unsigned int id, const char* uniform_name, float x, float y, float z);

void set_vec4(unsigned int id, const char* uniform_name, const vec4* value);

void set_vec4_f(unsigned int id, const char* uniform_name, float x, float y, float z, float w);

void set_mat2(unsigned int id, const char* uniform_name, const mat2* mat);

void set_mat3(unsigned int id, const char* uniform_name, const mat3* mat);

void set_mat4(unsigned int id, const char* uniform_name, const mat4* mat);
#endif // !EVOSHADER_H
