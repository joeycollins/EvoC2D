#ifndef GATHERER_H
#define GATHERER_H

#include "food.h"
#include "component.h"

void food_sensor(struct component* component, float* position);

void get_gps(struct component* component, float* value);

#endif