#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <stdio.h>
#include "evoshader.h"
#include "shapes.h"
#include "component.h"
#include "render.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

int main()
{
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

    unsigned int shaderProgram = build_shader("coloredvertexshader.vsh", "fragmentshader1.fsh");
    
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

    struct renderer render1 = create_creature_renderer(&s, shaderProgram);

    free_shape(&s);
    float delta_time = 0.0f, last_time = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float current_time = (float)glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        processInput(window);
        glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
      
        (*render1.draw)(&render1);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete_renderer(&render1);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

