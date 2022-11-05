#ifndef GATHERER_H_INCLUDED
#define GATHERER_H_INCLUDED
struct component;

void food_sensor(struct component* component, float* position);

void get_gps(struct component* component, float* value);

void get_eps(struct component* component, float* value);
#endif