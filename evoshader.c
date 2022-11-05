#define _CRT_SECURE_NO_WARNINGS
#include "evoshader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>
#include <stdbool.h>

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
	free((char*)vertex_shader_program);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &frag_shader_program, NULL);
	free((char*)frag_shader_program);
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
		long allocate_amt = file_size + 1;
		char* buffer = malloc((allocate_amt) * sizeof(char));
		if (buffer == 0) {
			free(buffer);
			fclose(file);
			return NULL;
		}
		size_t r = fread(buffer, 1, file_size, file);
		buffer[r] = '\0';
		fclose(file);
		return buffer;
	}
	else {
		printf("Shader file, %s, could not be opened\n", path);
		perror("");
	}
	return NULL;
}
