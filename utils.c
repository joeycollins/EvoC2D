#include "utils.h"
#include <stdlib.h>

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