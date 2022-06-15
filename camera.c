#include "camera.h"
#include <cglm/cglm.h>

void create_camera(struct camera* camera, int screen_width, int screen_height) {
	vec3 front = { 0.0f, 0.0f, -1.0f };
	vec3 up = { 0.0f, 1.0f, 0.0f };
	vec3 position = { 0.0f, 0.0f, 1.0f };
	glm_vec3_copy(front, camera->front);
	glm_vec3_copy(up, camera->up);
	glm_vec3_copy(position, camera->position);
}