#include "utils.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include "ShaderProgram.h"

std::string loadShaderFromFile(const std::string& shader_path)
{
	std::stringstream shader;

	std::ifstream fileIn;

	fileIn.open(shader_path, std::ifstream::in);

	std::string line;
	if (fileIn)
	{
		while (std::getline(fileIn, line))
		{
			shader << line + '\n';
		}
	}
	else
	{
		std::cout << "Error, '" << shader_path << "' not found." << std::endl;
	}

	return shader.str();
}

GLint createShader(const std::string& shader, GLuint shader_ptr)
{
	const GLchar* buffers = static_cast<const GLchar*>(shader.c_str());
	GLint shader_code_size = GLint(shader.size());
	GLint compile_status;

	glShaderSource(shader_ptr, 1, &buffers, &shader_code_size);
	glCompileShader(shader_ptr);
	glGetShaderiv(shader_ptr, GL_COMPILE_STATUS, &compile_status);
	if (GL_TRUE != compile_status)
	{
		GLint	infoLogLength;
		char*	strInfoLog;

		glGetShaderiv(shader_ptr, GL_INFO_LOG_LENGTH, &infoLogLength);
		strInfoLog = new char[infoLogLength + 1];
		glGetShaderInfoLog(shader_ptr, infoLogLength, NULL, strInfoLog);

		std::cout << strInfoLog << std::endl;

		delete[] strInfoLog;

		return compile_status;
	}

	return GL_TRUE;
}

GLuint createComputeShader(const std::string& shader, bool from_file)
{
	// Creating the compute shader, and the program object containing the shader
	GLuint progHandle = glCreateProgram();
	GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

	// In order to write to a texture, we have to introduce it as image2D.
	// local_size_x/y/z layout variables define the work group size.
	// gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
	// gl_LocalInvocationID is the local index within the work group, and
	// gl_WorkGroupID is the work group's index

	if (from_file)
	{
		std::string shader_source = loadShaderFromFile(shader);
		const GLchar* buffer = static_cast<const GLchar*>(shader_source.c_str());
		glShaderSource(cs, 1, &buffer, NULL);
	}
	else
	{
		const GLchar* buffer = static_cast<const GLchar*>(shader.c_str());
		glShaderSource(cs, 1, &buffer, NULL);
	}

	glCompileShader(cs);
	int rvalue;
	glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in compiling the compute shader\n");
		GLchar log[10240];
		GLsizei length;
		glGetShaderInfoLog(cs, 10239, &length, log);
		fprintf(stderr, "Compiler log:\n%s\n", log);
		
		return 0;
	}
	glAttachShader(progHandle, cs);

	glLinkProgram(progHandle);
	glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in linking compute shader program\n");
		GLchar log[10240];
		GLsizei length;
		glGetProgramInfoLog(progHandle, 10239, &length, log);
		fprintf(stderr, "Linker log:\n%s\n", log);

		return 0;
	}
	glUseProgram(progHandle);

	return progHandle;
}

void initializeRenderShader(ShaderProgram* shader)
{
	shader->bind();

	GLint posPtr = shader->getAttribLocation("pos");
	glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posPtr);
}

GLuint genTexture(int width, int height)
{
	std::cout << "GenTexture" << std::endl;

	GLuint texHandle;
	glGenTextures(1, &texHandle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	// Because we're also using this tex as an image (in order to write to it),
	// we bind it to an image unit as well
	glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	std::cout << "DONE" << std::endl;
	return texHandle;
}

glm::vec3 getCameraRay(const glm::vec3& camera_vector, float horizontal_angle, float vertical_angle)
{
	float sin_a = sin(-horizontal_angle);
	float cos_a = cos(-horizontal_angle);
	float sin_a_y = sin(-vertical_angle);
	float cos_a_y = cos(-vertical_angle);

	glm::vec3 ray_camera(camera_vector.x, camera_vector.y*cos_a_y - camera_vector.z*sin_a_y, camera_vector.y*sin_a_y + camera_vector.z*cos_a_y);
	return glm::vec3(ray_camera.x*cos_a - ray_camera.z*sin_a, ray_camera.y, ray_camera.x*sin_a + ray_camera.z*cos_a);
}

void checkErrors(std::string desc) 
{
	GLenum e = glGetError();
	if (e != GL_NO_ERROR)
	{
		fprintf(stderr, "OpenGL error in \"%s\": %s (%d)\n", desc.c_str(), gluErrorString(e), e);
	}
}

