#include "simulation.h"
#include "creature.h"
#include "settings.h"
#include "text.h"
#include "utils.h"
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

extern struct Simulation main_simulation;

static float last_time = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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

    if (simulation->window_mode == WINDOWED_FULLSCREEN) {
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        simulation->screen_width = mode->width;
        simulation->screen_height = mode->height;
        simulation->window = glfwCreateWindow(simulation->screen_width, simulation->screen_height,
            APP_NAME, primary_monitor, NULL);
    }else if (simulation->window_mode == FULLSCREEN) {
        simulation->screen_width = mode->width;
        simulation->screen_height = mode->height;
        simulation->window = glfwCreateWindow(simulation->screen_width, simulation->screen_height,
            APP_NAME, primary_monitor, NULL);
    }
    else {
        simulation->screen_width = DEFAULT_SCRN_WIDTH;
        simulation->screen_height = DEFAULT_SCRN_HEIGHT;
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
    glfwSetKeyCallback(simulation->window, key_callback);
    glfwSetMouseButtonCallback(simulation->window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initialize_text_character_set(); //add font

    create_camera(&simulation->main_camera, simulation->screen_width, simulation->screen_height); //create the main camera
    //want ui and sim to use different coord systems
    glm_ortho(0.0f, (float)simulation->screen_width, (float)simulation->screen_height, 0.0f, -1.0f, 1.0f, simulation->ortho);
    glm_ortho(0.0f, (float)simulation->screen_width, 0.0f, (float)simulation->screen_height, -1.0f, 1.0f, simulation->ui_projection);

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
        main_simulation.main_abyss_context.update(&main_simulation.main_abyss_context);
        //ui
        main_simulation.inspector.update(&main_simulation.inspector);

        
        glfwSwapBuffers(main_simulation.window);
        glfwPollEvents();
    }
    
    free_innovation_context(&main_simulation.main_innovation_context);
    free_structural_innovation_context(&main_simulation.main_structural_innovation_context);
}

unsigned int init_shader(const char* vertex_shader_path, const char* frag_shader_path, mat4 projection) {
    unsigned int res = build_shader(vertex_shader_path, frag_shader_path);
    use_shader(res);
    glUniformMatrix4fv(glGetUniformLocation(res, "projection"), 1, GL_FALSE, (GLfloat*)projection);
    return res;
}

void CreateAndInitSimulation() {
    main_simulation.window_mode = (enum window_mode)WINDOW_MODE;
    init(&main_simulation);
    main_simulation.Run = &run;
    main_simulation.draw_objects = true;
    main_simulation.shader_lib.creature_shader = init_shader("indivvertexshader.vsh", "fragmentshader1.fsh", main_simulation.ortho);
    main_simulation.shader_lib.food_shader = init_shader("coloredvertexshader.vsh", "fragmentshader1.fsh", main_simulation.ortho);
    main_simulation.shader_lib.text_shader = init_shader("textvertexshader.vsh", "textfragmentshader.fsh", main_simulation.ui_projection);
    main_simulation.shader_lib.inspector_panel_shader = init_shader("inspectorvertexshader.vsh", "inspectorfragmentshader.fsh", main_simulation.ui_projection);
    main_simulation.text_vao = create_text_vao();
    main_simulation.vao_pool = create_vao_pool(VAO_POOL_EBO_CAPACITY, VAO_POOL_VBO_CAPACITY, 1);
    main_simulation.main_structural_innovation_context = create_structural_innovation_context();
    main_simulation.main_innovation_context = get_new_innovation_context();
    main_simulation.main_food_context = create_food_context(FOOD_COUNT, FOOD_RADIUS, FOOD_COOLDOWN, main_simulation.shader_lib.food_shader);
    main_simulation.main_creature_context = create_creature_context(INITIAL_CREATURE_COUNT, &create_simple_creature, main_simulation.shader_lib.creature_shader);
    main_simulation.inspector = create_inspector(main_simulation.shader_lib.inspector_panel_shader, main_simulation.shader_lib.text_shader);
    main_simulation.main_abyss_context = create_abyss_context(ABYSS_COUNT, main_simulation.shader_lib.food_shader);
    main_simulation.creature_vao = create_individual_draw_mode_vao();
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    float speed = (float)(1000 * main_simulation.delta_time);
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_I && action == GLFW_PRESS) { //toggle the inspector panel
        main_simulation.inspector.target_creature = main_simulation.main_creature_context.creatures[0];
        main_simulation.inspector.enabled = !main_simulation.inspector.enabled;
    }
    else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        main_simulation.draw_objects = !main_simulation.draw_objects;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == 1) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        mat4 pos;
        glm_mat4_identity(pos);
        vec3 t = { xpos, ypos, 0 };
        glm_translate(pos, t);

        mat4 comp1;
        glm_mat4_inv(main_simulation.view, comp1);
        glm_mat4_mul(comp1, main_simulation.ortho, comp1);
        glm_mat4_mul(pos, comp1, comp1);

        struct creature* closest_creature = NULL;
        float closest = FLT_MAX;
        for (int i = 0; i < main_simulation.main_creature_context.creatures_count; i++) {
            struct creature* creature = main_simulation.main_creature_context.creatures[i];
            if (creature->life_stage == ALIVE) {
                float dist = mat4_distance_2d(comp1, creature->transform);
                if (dist < 500 && dist < closest) {
                    closest = dist;
                    closest_creature = creature;
                }
            }
        }
        if (closest_creature != NULL) {
            main_simulation.inspector.target_creature = closest_creature;
            main_simulation.inspector.enabled = true;
        }
    }
}

void reset_shader_orthos() {//reset projection in certain shaders on window resize
    use_shader(main_simulation.shader_lib.creature_shader);
    glUniformMatrix4fv(glGetUniformLocation(main_simulation.shader_lib.creature_shader, "projection"), 1, GL_FALSE, (GLfloat*)main_simulation.ortho);
    use_shader(main_simulation.shader_lib.text_shader);
    glUniformMatrix4fv(glGetUniformLocation(main_simulation.shader_lib.text_shader, "projection"), 1, GL_FALSE, (GLfloat*)main_simulation.ui_projection);
    use_shader(main_simulation.shader_lib.inspector_panel_shader);
    glUniformMatrix4fv(glGetUniformLocation(main_simulation.shader_lib.inspector_panel_shader, "projection"), 1, GL_FALSE, (GLfloat*)main_simulation.ui_projection);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    glm_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f, main_simulation.ortho);
    glm_ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f, main_simulation.ui_projection);
    reset_shader_orthos();
    main_simulation.screen_width = width;
    main_simulation.screen_height = height;
}