#include "utils.h"
#include <stdlib.h>
#include <cglm/cglm.h>

float rand_flt() {
	int random = rand();
	return (float)random / RAND_MAX;
}

int max_int(int a, int b) {
	return a > b ? a : b;
}

float rand_choice_flt(float a, float b) {
	if (rand_flt() < 0.5f) {
		return a;
	}
	return b;
}

void get_random_translation(float origin_x, float origin_y, float radius, mat4 dest) {
	float x = rand_flt() * radius * cos(rand_flt() * 2 * M_PI);
	float y = rand_flt() * radius * sin(rand_flt() * 2 * M_PI);
	vec3 translation = { x, y , 0.0f };
	glm_mat4_identity(dest);
	glm_translate(dest, translation);
}

void get_translation_matrix(mat4 source, mat4 dest) {
	mat4 new_mat;
	glm_mat4_identity(new_mat);
	new_mat[3][0] = source[3][0];
	new_mat[3][1] = source[3][1];
	new_mat[3][2] = source[3][2];
	glm_mat4_copy(new_mat, dest);
}