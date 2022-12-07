#ifndef ACTUALIZER_H_INCLUDED
#define ACTUALIZER_H_INCLUDED

struct component;

void thruster(struct component* component, float* value);

void rotator(struct component* component, float* value);

void asexual_reproduction(struct component* component, float* value);

void sexual_reproduction(struct component* component, float* value);

void do_fangs(struct component* component, float* value);

#endif