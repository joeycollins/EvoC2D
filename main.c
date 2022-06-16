#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include "food.h"
#include "evoshader.h"
#include "shapes.h"
#include "component.h"
#include "render.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

float delta_time = 0.0f, last_time = 0.0f;

struct camera main_camera;

int main()
{
    srand(time(NULL)); //seed

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
    glm_ortho(0.0f, SCR_WIDTH, SCR_HEIGHT, 0.0f, -1.0f, 1.0f, ortho);
    
    unsigned int shaderProgram = build_shader("coloredvertexshader.vsh", "fragmentshader1.fsh");
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, ortho); //set otho uniform in shader

    struct component comp = {
        .children_count = 1,
        .color = {0.2f, 0.9f, 0.0f}
    };

    struct component comp2 = {
        .children_count = 3,
        .color = {0.7f, 0.0f, 7.0f}
    };

    struct component comp3 = {
        .children_count = 0,
        .color = {1.0f, 0.0f, 1.0f}
    };

    struct component comp4 = {
        .children_count = 0,
        .color = {0.0f, 0.3f, 1.0f}
    };

    struct component comp5 = {
        .children_count = 0,
        .color = {1.0f, 0.0f, 0.0f}
    };

    comp.children[0] = &comp2;
    comp2.children[0] = &comp3;
    comp2.children[1] = &comp4;
    comp2.children[2] = &comp5;

    struct creature creature = create_creature("molly", comp);
    struct shape s = create_creature_model(&creature, true);

    struct renderer render1 = create_renderer(&s, shaderProgram);

    struct food_context food1 = create_food_context(100, 1200);
    struct renderer food_rend = create_renderer(&food1.shape, shaderProgram);

    mat4 trans;
    glm_mat4_identity(trans);
    vec3 scale = { 400, 400, 1.0f };
    glm_scale(trans, scale);

    free_shape(&s);
    free_shape(&food1.shape);
    
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
       

        for (int i = 0; i < food1.capacity; i++) {
            render(&food1.food[i].transform, &food_rend, view);
        }
        
        render(&trans, &render1, view);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete_renderer(&render1);
    delete_renderer(&food1);
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

