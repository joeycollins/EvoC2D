#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include "creaturecontext.h"
#include "innovation.h"
#include "inspector.h"
#include "food.h"
#include "camera.h"
#include "render.h"
#include "evoshader.h"
#include <stdbool.h>
#include <cglm/mat4.h>


struct organism; struct GLFWwindow; typedef struct GLFWwindow GLFWwindow;

//Singleton that manages entity memory, global variables, delta time etc
struct Simulation {
	float delta_time;
	struct camera main_camera;
	struct creature_context main_creature_context;
	struct innovation_context main_innovation_context;
	struct structural_innovation_context main_structural_innovation_context;
	struct inspector inspector;
	struct food_context main_food_context;
	void(*Run)(void);
	bool fullscreen;
	unsigned int screen_width;
	unsigned int screen_height;
	mat4 view;
	mat4 ortho;
	mat4 ui_projection;
	GLFWwindow* window;
	unsigned int current_vao;
	unsigned int current_shader;
	struct vao text_vao;
	struct vao_pool vao_pool;
	struct shader_lib shader_lib;
};

void CreateAndInitSimulation();


#endif