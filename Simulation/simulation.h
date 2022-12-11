#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include "creaturecontext.h"
#include "innovation.h"
#include "inspector.h"
#include "food.h"
#include "camera.h"
#include "render.h"
#include "evoshader.h"
#include "abysscontext.h"
#include <stdbool.h>
#include <cglm/mat4.h>

struct organism; struct GLFWwindow; typedef struct GLFWwindow GLFWwindow;

enum window_mode {
	FULLSCREEN = 0,
	WINDOWED_FULLSCREEN = 1,
	WINDOWED = 2
};

//Singleton that manages entity memory, global variables, delta time etc
struct Simulation {
	float delta_time;
	bool draw_objects;
	struct camera main_camera;
	struct creature_context main_creature_context;
	struct innovation_context main_innovation_context;
	struct structural_innovation_context main_structural_innovation_context;
	struct abyss_context main_abyss_context;
	struct inspector inspector;
	struct food_context main_food_context;
	void(*Run)(void);
	enum window_mode window_mode;
	unsigned int screen_width;
	unsigned int screen_height;
	mat4 view;
	mat4 ortho;
	int zoom;
	mat4 ui_projection;
	GLFWwindow* window;
	unsigned int current_vao;
	unsigned int current_shader;
	struct vao text_vao;
	struct vao creature_vao;
	struct vao_pool vao_pool;
	struct shader_lib shader_lib;
};

void CreateAndInitSimulation();


#endif