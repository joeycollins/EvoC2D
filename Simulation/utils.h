#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cglm/mat4.h>

float rand_flt();
int max_int(int a, int b);
float rand_choice_flt(float a, float b);
void get_random_translation(float origin_x, float origin_y, float radius, mat4 dest);
void get_translation_matrix(mat4 source, mat4 dest);
float normalize(float range_min, float range_max, float value_min, float value_max, float value);
float quick_magnitude_2d(mat4 transform1, mat4 transform2);
float mat4_distance_2d(mat4 transform1, mat4 transform2);
char* int_to_string(int value);
char* float_to_string(float value);
void get_random_color(float* rdest, float* gdest, float* bdest);
char* generate_random_name(char name[12]);
#endif