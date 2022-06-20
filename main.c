#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <time.h>
#include "globals.h"
#include "food.h"
#include "innovation.h"
#include "evoshader.h"
#include "shapes.h"
#include "component.h"
#include "render.h"
#include "camera.h"
#include "genome.h"
#include "multilayerperceptron.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

float delta_time = 0.0f, last_time = 0.0f;

struct camera main_camera;

struct food_context main_food_context;
struct innovation_context main_innovation_context;

int main()
{
    srand((unsigned int)time(NULL)); //seed
    
    main_food_context = create_food_context(100, 1200);
    main_innovation_context = get_new_innovation_context();

                                     // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "EvoC2D", NULL, NULL);

    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    create_camera(&main_camera, SCR_WIDTH, SCR_HEIGHT); //create the main camera

    mat4 ortho;
    glm_ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f, ortho);
    
    unsigned int shaderProgram = build_shader("coloredvertexshader.vsh", "fragmentshader1.fsh");
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (GLfloat*)ortho); //set otho uniform in shader

   
 
    struct creature creature;
    create_simple_creature(&creature);
    struct shape s = create_creature_model(&creature, true);

    struct genome genome = get_new_genome(&creature);
    mutate_add_connection(&genome, &main_innovation_context, false);
    mutate_add_gene(&genome, &main_innovation_context); //fix

    struct multilayer_perceptron network = create_multilayer_perceptron(&genome);
    evaluate(&network);

    struct renderer render1 = create_renderer(&s, shaderProgram);

    struct renderer food_rend = create_renderer(&main_food_context.shape, shaderProgram);

    mat4 trans;
    glm_mat4_identity(trans);
    vec3 scale = { 400, 400, 1.0f };
    glm_scale(trans, scale);

    free_shape(&s);
    free_shape(&main_food_context.shape);
    
    while (!glfwWindowShouldClose(window))
    {
        float current_time = (float)glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        processInput(window);
        glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //calculate the camera view for the frame
        vec3 center;
        mat4 view;
        glm_vec3_add(main_camera.position, main_camera.front, center);
        glm_lookat(main_camera.position, center, main_camera.up, view);

        //render models
       

        for (int i = 0; i < main_food_context.capacity; i++) {
            render(&main_food_context.food[i].transform, &food_rend, view);
        }
        
        render(&trans, &render1, view);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete_renderer(&render1);
    delete_renderer(&food_rend);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    float speed = (float)(200 * delta_time);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 up = { 0.0f, -1.0f, 0.0f };
        glm_vec3_scale(up, speed, up);
        glm_vec3_add(main_camera.position, up, main_camera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 down = { 0.0f, 1.0f, 0.0f };
        glm_vec3_scale(down, speed, down);
        glm_vec3_add(main_camera.position, down, main_camera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 left = { -1.0f, 0.0f, 0.0f };
        glm_vec3_scale(left, speed, left);
        glm_vec3_add(main_camera.position, left, main_camera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 right = { 1.0f, 0.0f, 0.0f };
        glm_vec3_scale(right, speed, right);
        glm_vec3_add(main_camera.position, right, main_camera.position);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

