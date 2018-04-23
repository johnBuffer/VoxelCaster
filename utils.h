#pragma once

#include <string>
#include <GL/glew.h>

class ShaderProgram;

std::string loadShaderFromFile(const std::string& shader_path);

GLint createShader(const std::string& shader, GLuint shader_ptr);

GLuint createComputeShader(const std::string& shader, bool from_file=false);

void initializeRenderShader(ShaderProgram* shader);

GLuint genTexture(int width, int height);

void checkErrors(std::string desc="");

