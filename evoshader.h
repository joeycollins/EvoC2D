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

#endif // !EVOSHADER_H
