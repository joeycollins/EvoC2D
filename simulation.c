#include "simulation.h"
#include "creature.h"
#include "settings.h"
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

static float last_time = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void init(struct Simulation* simulation) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    srand((unsigned int)time(NULL)); //seed

    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
    
    if (simulation->fullscreen) {
        simulation->screen_width = mode->width;
        simulation->screen_height = mode->height;
        simulation->window = glfwCreateWindow(simulation->screen_width, simulation->screen_height,
            APP_NAME, primary_monitor, NULL);
    }
    else {
        simulation->window = glfwCreateWindow(simulation->screen_width, simulation->screen_height,
            APP_NAME, NULL, NULL);
    }

    if (simulation->window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(simulation->window);
    glfwSetFramebufferSizeCallback(simulation->window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    create_camera(&simulation->main_camera, simulation->screen_width, simulation->screen_height); //create the main camera
    glm_ortho(0.0f, (float)simulation->screen_width, (float)simulation->screen_height, 0.0f, -1.0f, 1.0f, simulation->ortho);
}

void run() {
    while (!glfwWindowShouldClose(main_simulation.window))
    {
        float current_time = (float)glfwGetTime();
        main_simulation.delta_time = current_time - last_time;
        last_time = current_time;

        processInput(main_simulation.window);
        glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //calculate the camera view for the frame
        vec3 center;
        glm_vec3_add(main_simulation.main_camera.position, main_simulation.main_camera.front, center);
        glm_lookat(main_simulation.main_camera.position, center, main_simulation.main_camera.up, main_simulation.view);

        //updates
        main_simulation.main_creature_context.update(&main_simulation.main_creature_context);
        main_simulation.main_food_context.update(&main_simulation.main_food_context);

        glfwSwapBuffers(main_simulation.window);
        glfwPollEvents();
    }
}

unsigned int init_shader(const char* vertex_shader_path, const char* frag_shader_path) {
    unsigned int res = build_shader(vertex_shader_path, frag_shader_path);
    use_shader(res);
    glUniformMatrix4fv(glGetUniformLocation(res, "projection"), 1, GL_FALSE, (GLfloat*)main_simulation.ortho);
    return res;
}

void CreateAndInitSimulation() {
    main_simulation.fullscreen = FULLSCREEN;
    init(&main_simulation);
    main_simulation.Run = &run;
    main_simulation.shader_lib.creature_shader = init_shader("coloredvertexshader.vsh", "fragmentshader1.fsh");
    main_simulation.shader_lib.food_shader = main_simulation.shader_lib.creature_shader;
    main_simulation.vao_pool = create_vao_pool(VAO_POOL_EBO_CAPACITY, VAO_POOL_VBO_CAPACITY, 1);
    main_simulation.main_structural_innovation_context = create_structural_innovation_context();
    main_simulation.main_innovation_context = get_new_innovation_context();
    main_simulation.main_food_context = create_food_context(FOOD_COUNT, FOOD_RADIUS, FOOD_COOLDOWN, main_simulation.shader_lib.food_shader);
    main_simulation.main_creature_context = create_creature_context(INITIAL_CREATURE_COUNT, &create_simple_creature_2, main_simulation.shader_lib.creature_shader);
    
    
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    float speed = (float)(800 * main_simulation.delta_time);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 up = { 0.0f, -1.0f, 0.0f };
        glm_vec3_scale(up, speed, up);
        glm_vec3_add(main_simulation.main_camera.position, up, main_simulation.main_camera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 down = { 0.0f, 1.0f, 0.0f };
        glm_vec3_scale(down, speed, down);
        glm_vec3_add(main_simulation.main_camera.position, down, main_simulation.main_camera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 left = { -1.0f, 0.0f, 0.0f };
        glm_vec3_scale(left, speed, left);
        glm_vec3_add(main_simulation.main_camera.position, left, main_simulation.main_camera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 right = { 1.0f, 0.0f, 0.0f };
        glm_vec3_scale(right, speed, right);
        glm_vec3_add(main_simulation.main_camera.position, right, main_simulation.main_camera.position);
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    glm_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f, main_simulation.ortho);
    main_simulation.screen_width = width;
    main_simulation.screen_height = height;
}