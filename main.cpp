#include <iostream>
#include <vector>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Grid2D.h"
#include "Grid3D.h"
#include "ShaderProgram.h"
#include "utils.h"

int main()
{
    int WIN_WIDTH = 1000;
    int WIN_HEIGHT = 1000;

	int RENDER_WIDTH = 500;
	int RENDER_HEIGHT = 500;

	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Discard old OpenGL

	// Ouvre une fenêtre et crée son contexte OpenGl
	GLFWwindow* window;
	window = glfwCreateWindow(512, 512, "vxc", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Nécessaire dans le profil de base
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	/*
		Compute things
	*/

	GLuint texture = genTexture(512, 512);
	GLuint compute_shader = createComputeShader("test.cp", true);

	glUseProgram(compute_shader);
	//GLint dest_text_location = glGetUniformLocation(compute_shader, "img_output");
	//glUniform1i(dest_text_location, tex_output);

	// DATA
	const int grid_size_x = 100;
	const int grid_size_y = 100;
	const int grid_size_z = 1;

	const int grid_size[] = { grid_size_x, grid_size_y, grid_size_z };
	int grid[ grid_size_x * grid_size_y * grid_size_z ];

	for (int i(0); i<20; ++i)
		grid[100 * 50 + i] = 1;

	std::cout << sizeof(grid) << std::endl;

	GLuint grid_size_buffer;
	glGenBuffers(1, &grid_size_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, grid_size_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 3 * sizeof(int), grid_size, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grid_size_buffer);
	
	GLuint grid_buffer;
	glGenBuffers(1, &grid_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, grid_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size_x*grid_size_y*grid_size_z * sizeof(int), grid, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, grid_buffer);
	
	ShaderProgram shader(ShaderProgram::base_vertex, ShaderProgram::base_fragment, true);
	//shader.setUniform("srcTex", tex_output);

	/*
		Create Quad object
	*/
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	float data[] = {
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, data, GL_STATIC_DRAW);

	shader.bind();
	GLint posPtr = shader.getAttribLocation("pos");
	glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posPtr);

	// Assure que l'on peut capturer la touche d'échappement enfoncée ci-dessous
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{
		glUseProgram(compute_shader);
		glDispatchCompute((GLuint)512, (GLuint)512, 1);

		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		// normal drawing pass
		glClear(GL_COLOR_BUFFER_BIT);
		shader.bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		checkErrors("lol");
	}

    return 0;
}
