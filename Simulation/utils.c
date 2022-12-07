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

float normalize(float range_min, float range_max, float value_min, float value_max, float value) {
	return (value - value_min) / (value_max - value_min) * (range_max - range_min) + range_min;

}

float quick_magnitude_2d(mat4 transform1, mat4 transform2) {
	float delta_x = transform2[3][0] - transform1[3][0];
	float delta_y = transform2[3][1] - transform1[3][1];
	return (delta_x * delta_x) + (delta_y * delta_y);
}

float mat4_distance_2d(mat4 transform1, mat4 transform2) {
	float delta_x = transform2[3][0] - transform1[3][0];
	float delta_y = transform2[3][1] - transform1[3][1];
	return sqrtf((delta_x * delta_x) + (delta_y * delta_y));
}

char* float_to_string(float value) {
	int len = snprintf(NULL, 0, "%f", value);
	char* dest = malloc(len + 1);
	snprintf(dest, len + 1, "%f", value);
	return dest;
}

char* int_to_string(int value) {
	int len = snprintf(NULL, 0, "%d", value);
	char* dest = malloc(len + 1);
	snprintf(dest, len + 1, "%d", value);
	return dest;
}

void get_random_color(float* rdest, float* gdest, float* bdest) {
	float x = rand(), y = rand(), z = rand();
	*rdest = x / RAND_MAX;
	*gdest = y / RAND_MAX;
	*bdest = z / RAND_MAX;
}