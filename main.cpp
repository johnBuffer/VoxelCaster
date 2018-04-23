#include <iostream>
#include <vector>
#include "Grid3D.h"
#include "utils.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

int main()
{
    int WIN_WIDTH = 1000;
    int WIN_HEIGHT = 1000;

	int RENDER_WIDTH = 500;
	int RENDER_HEIGHT = 500;

    Grid3D grid(40, 40, 40, 25);

	int work_grp_cnt[3];
    
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
    

    return 0;
}
