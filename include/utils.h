#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class ShaderProgram;

std::string loadShaderFromFile(const std::string& shader_path);

GLint createShader(const std::string& shader, GLuint shader_ptr);

GLuint createComputeShader(const std::string& shader, bool from_file=false);

void initializeRenderShader(ShaderProgram* shader);

GLuint genTexture(int width, int height);

glm::vec3 getCameraRay(const glm::vec3& camera_vector, float horizontal_angle, float vertical_angle);

void checkErrors(std::string desc="");

template<class T>
T& getElementAt(std::vector<T>& data, int size_x, int size_y, int size_z, int x, int y, int z)
{
	int grid_index = x * (size_y*size_z) + y * size_z + z;
	return data[grid_index];
}


