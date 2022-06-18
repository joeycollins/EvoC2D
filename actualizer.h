#ifndef ACTUALIZER_H
#define ACTUALIZER_H

#include "component.h"
#include "creature.h"
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

void thruster(struct component* component, vec2 value);

void rotator(struct component* component, float value);


#endif