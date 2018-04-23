#include <iostream>
#include <vector>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Grid2D.h"
#include "Grid3D.h"
#include "ShaderProgram.h"
#include "utils.h"
#include "FastNoise.h"

int main()
{
    int WIN_WIDTH = 1024;
    int WIN_HEIGHT = 1024;

	int RENDER_WIDTH = 512;
	int RENDER_HEIGHT = 512;

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
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "vxc", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSwapInterval(0);

	// Initialize GLEW
	glewExperimental = true; // Nécessaire dans le profil de base
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	/*
		Compute things
	*/

	GLuint texture = genTexture(RENDER_WIDTH, RENDER_HEIGHT);
	GLuint compute_shader = createComputeShader("test.cs", true);

	glUseProgram(compute_shader);
	glUniform2i(4, RENDER_WIDTH, RENDER_HEIGHT);
	//GLint dest_text_location = glGetUniformLocation(compute_shader, "img_output");
	
	glm::vec3 start_position(100, 100, 100);

	// DATA
	const int grid_size_x = 1000;
	const int grid_size_y = 100;
	const int grid_size_z = 1000;

	Grid3D grid(grid_size_x, grid_size_y, grid_size_z, 10);

	const int grid_size[] = { grid_size_x, grid_size_y, grid_size_z };

	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type

	for (int x = 0; x < grid_size_x; x++)
	{
		for (int z = 0; z < grid_size_z; z++)
		{
			int max_height = 50;
			int height = max_height / 2 * myNoise.GetNoise(x, z);

			for (int y(0); y < max_height + height; ++y)
			{
				grid(x, y, z) = 1;
			}
		}
	}

	/*for (int i(100000); i--;)
	{
		int x = rand() % grid_size_x;
		int y = rand() % grid_size_y;
		int z = rand() % grid_size_z;

		int grid_index = x * (grid_size_y*grid_size_z) + y * grid_size_z + z;
		grid[grid_index] = 1;
	}*/

	float camera_horizontal_angle = 0.0;
	float camera_vertical_angle = 0.0;

	GLuint grid_size_buffer;
	glGenBuffers(1, &grid_size_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, grid_size_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 3 * sizeof(int), grid_size, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grid_size_buffer);
	
	GLuint grid_buffer;
	glGenBuffers(1, &grid_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, grid_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, grid_size_x*grid_size_y*grid_size_z * sizeof(int), grid.data(), GL_STATIC_DRAW);
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

	glm::vec3 camera_origin(WIN_WIDTH / 2, WIN_HEIGHT / 2, -WIN_WIDTH / 1);
	glm::vec3 camera_vec(0, 0, -1);
	//camera_vec = glm::normalize(camera_vec);

	// Assure que l'on peut capturer la touche d'échappement enfoncée ci-dessous
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{
		glUseProgram(compute_shader);

		glm::vec3 movement_vec = getCameraRay(camera_vec, camera_horizontal_angle, camera_vertical_angle);
		movement_vec.y = -movement_vec.y;

		glm::vec3 movement = glm::vec3(0.0f, 0.0f, 0.0f);

		// Keys management
		int state = glfwGetKey(window, GLFW_KEY_W);
		if (state == GLFW_PRESS)
		{
			movement = -5.0f*movement_vec;
		}

		state = glfwGetKey(window, GLFW_KEY_S);
		if (state == GLFW_PRESS)
		{
			movement = 5.0f*movement_vec;
		}

		state = glfwGetKey(window, GLFW_KEY_SPACE);
		if (state == GLFW_PRESS)
		{
			start_position.y += 5;
		}

		state = glfwGetKey(window, GLFW_KEY_A);
		if (state == GLFW_PRESS)
		{
			start_position.x -= 5;
		}

		/*if (glm::length(movement) > 0.0f)
		{
			HitPoint3D move_test = grid.castRay(start_position, glm::normalize(movement));
			if (move_test.m_hit)
			{
				glm::vec3 next_position = start_position + movement;
				if (glm::length(start_position - move_test.m_point) < glm::length(movement))
				{
					start_position = move_test.m_point;
				}
				else
				{
					start_position = next_position;
				}
			}
			else
			{
				start_position += movement;
			}
		}*/
		start_position += movement;

		glUniform3f(2, start_position.x, start_position.y, start_position.z);

		// Mouse management
		double mouse_x_pos, mouse_y_pos;
		glfwGetCursorPos(window, &mouse_x_pos, &mouse_y_pos);
		camera_horizontal_angle = (mouse_x_pos - RENDER_WIDTH / 2)*0.01;
		camera_vertical_angle   = (mouse_y_pos - RENDER_WIDTH / 2)*0.01;
		glUniform2f(3, camera_horizontal_angle, camera_vertical_angle);

		glDispatchCompute((GLuint)RENDER_WIDTH, (GLuint)RENDER_HEIGHT, 1);

		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		// normal drawing pass
		glClear(GL_COLOR_BUFFER_BIT);
		shader.bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		//checkErrors("lol");
	}

    return 0;
}
