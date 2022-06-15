#ifndef  CAMERA_H
#define CAMERA_H

#include <cglm/vec3.h>

//simple 2d panning camera
struct camera {
	vec3 position;
	vec3 up;
	vec3 front;
};

void create_camera(struct camera* camera, int screen_width, int screen_height);

#endif // ! CAMERA_H
