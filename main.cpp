#include <iostream>
#include <vector>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Octree.h"
#include "ShaderProgram.h"
#include "utils.h"
#include "FastNoise.h"

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
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	
	glm::vec3 start_position(0, 5, -1);

	FastNoise myNoise; // Create a FastNoise object

	float camera_horizontal_angle = 0.0;
	float camera_vertical_angle = 0.0;
	
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

	glm::vec3 camera_vec(0, 0, 1);
	//camera_vec = glm::normalize(camera_vec);
	glm::vec3 light_position;

	int use_opti = 0;

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{
		time += 0.005;

		light_position = glm::vec3(100 * cos(time), 0.0, 100 * sin(time));

		glUseProgram(compute_shader);

		// Mouse management
		double mouse_x_pos, mouse_y_pos;
		glfwGetCursorPos(window, &mouse_x_pos, &mouse_y_pos);
		camera_horizontal_angle = (mouse_x_pos - RENDER_WIDTH / 2)*0.0025;
		camera_vertical_angle = (mouse_y_pos - RENDER_WIDTH / 2)*0.0025;

		glm::mat3 rotY = { cos(camera_horizontal_angle), 0, sin(camera_horizontal_angle),
						  0, 1, 0,
						  -sin(camera_horizontal_angle), 0, cos(camera_horizontal_angle) };

		glm::mat3 rotX = { 1.0, 0.0, 0.0,
						  0.0, cos(camera_vertical_angle), -sin(camera_vertical_angle),
						  0.0, sin(camera_vertical_angle), cos(camera_vertical_angle) };

		glm::mat3 rot_mat = rotX * rotY;

		glm::vec3 movement = glm::vec3(0.0f, 0.0f, 0.0f);

		float speed = 0.05f;

		// Keys management
		int state = glfwGetKey(window, GLFW_KEY_W);
		if (state == GLFW_PRESS)
		{
			movement = glm::vec3(0.0f, 0.0f, speed);
		}

		state = glfwGetKey(window, GLFW_KEY_S);
		if (state == GLFW_PRESS)
		{
			movement = glm::vec3(0.0f, 0.0f, -speed);
		}

		start_position += glm::transpose(rot_mat)*movement;
		
		//std::cout << "Opti: " << use_opti << std::endl;
		glUniform3f(2, start_position.x, start_position.y, start_position.z);
		glUniform3f(5, light_position.x, light_position.y, light_position.z);

		glUniformMatrix3fv(6, 1, GL_TRUE, glm::value_ptr(rot_mat));

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
