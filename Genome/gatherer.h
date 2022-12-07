#ifndef GATHERER_H_INCLUDED
#define GATHERER_H_INCLUDED
struct component;

void food_sensor(struct component* component, float* position);

void get_eps(struct component* component, float* value);

void creature_sensor(struct component* component, float* position);

void abyss_sensor(struct component* component, float* position);
#endif