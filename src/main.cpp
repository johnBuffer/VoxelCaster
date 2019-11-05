#include <iostream>
#include <vector>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Octree.h"
#include "ShaderProgram.h"
#include "utils.h"
#include <FastNoise.h>

int main()
{
    int WIN_WIDTH = 1024;
    int WIN_HEIGHT = 1024;

	int RENDER_WIDTH = 1024;
	int RENDER_HEIGHT = 1024;

	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 1); // 4x antialiasing
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
	GLuint compute_shader = createComputeShader("gpu_raycaster.comp", true);

	glUseProgram(compute_shader);
	glUniform2i(4, RENDER_WIDTH, RENDER_HEIGHT);
	//GLint dest_text_location = glGetUniformLocation(compute_shader, "img_output");
	
	glm::vec3 start_position(450, 600, 450);

	// DATA
	const int octree_size = 1024;

	Octree octree;
	octree.addElement(500, 500, 500);
	/*octree.addElement(502, 500, 500);
	octree.addElement(504, 500, 500);
	octree.addElement(506, 500, 500);*/

	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type

	for (int x = 0; x < octree_size; x++)
	{
		for (int z = 0; z < octree_size; z++)
		{
			int max_height = 25;
			int height = max_height / 2 * myNoise.GetNoise(x, z);

			for (int y(0); y < max_height + height; ++y)
			{
				octree.addElement(x, y, z);
			}
		}
	}

	/*for (int i(100); i--;)
	{
		octree.addElement(0, 0, 0);
	}*/

	/*while (octree.getSize() < 20000)
	{
		octree.addElement(rand() % octree_size, rand() % octree_size, rand() % octree_size);
	}*/

	std::cout << "Octree size: " << octree.getSize() << std::endl;

	/*for (int x = 0; x < grid_size_x; x++)
	{
		for (int z = 0; z < grid_size_z; z++)
		{
			int max_height = 25;
			int height = max_height / 2 * myNoise.GetNoise(x, z);

			for (int y(0); y < max_height + height; ++y)
			{
				grid(x, y, z) = 1;
			}
		}
	}*/

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
	
	GLuint octree_buffer;
	glGenBuffers(1, &octree_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, octree_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, octree.getSize() * sizeof(OctreeElement), octree.getData(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, octree_buffer);
	
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

	double time = 0.0;

	glm::vec3 camera_origin(WIN_WIDTH / 2, WIN_HEIGHT / 2, -WIN_WIDTH / 1);
	glm::vec3 camera_vec(0, 0, -1);
	//camera_vec = glm::normalize(camera_vec);
	glm::vec3 light_position;

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{
		time += 0.005;

		light_position = glm::vec3(500 * cos(time) + 500, 500+200*sin(time), 500 * sin(time) + 500);

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
		glUniform3f(5, light_position.x, light_position.y, light_position.z);

		// Mouse management
		double mouse_x_pos, mouse_y_pos;
		glfwGetCursorPos(window, &mouse_x_pos, &mouse_y_pos);
		camera_horizontal_angle = (mouse_x_pos - RENDER_WIDTH / 2)*0.005;
		camera_vertical_angle   = (mouse_y_pos - RENDER_WIDTH / 2)*0.005;
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
