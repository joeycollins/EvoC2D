#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cglm/mat4.h>

float rand_flt();
int max_int(int a, int b);
float rand_choice_flt(float a, float b);
void get_random_translation(float origin_x, float origin_y, float radius, mat4 dest);
void get_translation_matrix(mat4 source, mat4 dest);
#endif