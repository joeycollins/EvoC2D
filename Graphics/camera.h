#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <cglm/vec3.h>

//simple 2d panning camera
struct camera {
	vec3 position;
	vec3 up;
	vec3 front;
};

void init_camera(struct camera* camera);

#endif // ! CAMERA_H
