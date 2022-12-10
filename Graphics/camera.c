#include "camera.h"
#include <cglm/cglm.h>

void init_camera(struct camera* camera) {
	vec3 front = { 0.0f, 0.0f, -1.0f };
	vec3 up = { 0.0f, 1.0f, 0.0f };
	vec3 position = { 0.0f, 0.0f, 1.0f };
	glm_vec3_copy(front, camera->front);
	glm_vec3_copy(up, camera->up);
	glm_vec3_copy(position, camera->position);
}