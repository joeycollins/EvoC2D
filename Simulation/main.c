#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <time.h>
#include "globals.h"
#include "simulation.h"
/*
Quick architecture overview:

Entity objects control all self-logic and rendering; creatures, food etc. Contexts should update entities. 
Sub entities like renderers, neural nets, genomes, point upwards. Reallocing should always be safe for
any object before creating any sub entity. Reallocing should never occur after a subentity has been created.

Ex. Reallocing a creatures component array is safe until that component creates a gene, which is a sub entity of 
 component.

Creature -> Component -> IO Gene -> Connection -> NN
            Genome -> Hidden Gene
            
Renderer


*/


float creature_scale_factor = 400.0f, food_scale_factor = 100.0f;

struct Simulation main_simulation;

int main()
{
    CreateAndInitSimulation();
    main_simulation.Run();
    glfwTerminate();
    return 0;
}
