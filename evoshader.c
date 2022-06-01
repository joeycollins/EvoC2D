#define _CRT_SECURE_NO_WARNINGS
#include "evoshader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

const char* read_file(const char* path);

unsigned int build_shader(const char* vertex_shader_path, const char* fragment_shader_path) {
    const char* vertex_shader_program = read_file(vertex_shader_path);
	const char* frag_shader_program = read_file(fragment_shader_path);

	//compile
	unsigned int vertex;
	unsigned int fragment;
	int success;
	char info[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_shader_program, NULL);
	free(vertex_shader_program);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &frag_shader_program, NULL);
	free(frag_shader_program);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
	}

	unsigned int ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, info);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return ID;
}

//reads shader code and places it in program string, returns char count of program
const char* read_file(const char* path) {
	FILE* file;
	if (file = fopen(path, "r")) {
		fseek(file, 0, SEEK_END);
		long file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
		char* buffer = malloc((file_size + 1) * sizeof(char));
		if (buffer == 0) {
			free(buffer);
			fclose(file);
			return NULL;
		}
		int r = fread(buffer, 1, file_size, file);
		buffer[r] = '\0';
		fclose(file);
		return buffer;
	}
	else {
		fprintf("Shader file, %s, could not be opened\n", path);
	}
	return NULL;
}

void use_shader(unsigned int id) {
	glUseProgram(id);
}

void set_uniform_bool(unsigned int id, const char* uniform_name, bool value) {
	glUniform1i(glGetUniformLocation(id, uniform_name), (int)value);
}

void set_uniform_int(unsigned int id, const char* uniform_name, int value) {
	glUniform1i(glGetUniformLocation(id, uniform_name), value);
}

void set_uniform_float(unsigned int id, const char* uniform_name, float value) {
	glUniform1f(glGetUniformLocation(id, uniform_name), value);
}

void set_vec2(unsigned int id, const char* uniform_name, const vec2* value)
{
	glUniform2fv(glGetUniformLocation(id, uniform_name), 1, &value[0]);
}

void set_vec2_f(unsigned int id, const char* uniform_name, float x, float y)
{
	glUniform2f(glGetUniformLocation(id, uniform_name), x, y);
}

void set_vec3(unsigned int id, const char* uniform_name, const vec3* value)
{
	glUniform3fv(glGetUniformLocation(id, uniform_name), 1, &value[0]);
}

void set_vec3_f(unsigned int id, const char* uniform_name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(id, uniform_name), x, y, z);
}

void set_vec4(unsigned int id, const char* uniform_name, const vec4* value)
{
	glUniform4fv(glGetUniformLocation(id, uniform_name), 1, &value[0]);
}

void set_vec4_f(unsigned int id, const char* uniform_name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(id, uniform_name), x, y, z, w);
}

void set_mat2(unsigned int id, const char* uniform_name, const mat2* mat)
{
	glUniformMatrix2fv(glGetUniformLocation(id, uniform_name), 1, GL_FALSE, &mat[0][0]);
}

void set_mat3(unsigned int id, const char* uniform_name, const mat3* mat)
{
	glUniformMatrix3fv(glGetUniformLocation(id, uniform_name), 1, GL_FALSE, &mat[0][0]);
}

void set_mat4(unsigned int id, const char* uniform_name, const mat4* mat)
{
	glUniformMatrix4fv(glGetUniformLocation(id, uniform_name), 1, GL_FALSE, &mat[0][0]);
}