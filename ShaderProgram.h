#pragma once

#include <map>
#include <string>
#include <glm\glm.hpp>

#include "utils.h"

class ShaderProgram
{
public:
	ShaderProgram(bool is_default=false);
	ShaderProgram(const std::string& vertex_filename, const std::string& fragment_filename, bool import_from_memory=false);

	virtual ~ShaderProgram() = default;

	GLint getUniformLocation(const std::string& uniform_name) const;
	GLint getAttribLocation(const std::string& attrib_name) const;
	
	void setUniform(const std::string& uniform_name, GLint value) const;

	void setUniform(GLint uniform_location, const glm::mat4& mat) const;
	void setUniform(GLint uniform_location, float value) const;
	void setUniform(GLint uniform_location, const glm::vec3& value) const;
	void setUniform(GLint uniform_location, const glm::vec4& value) const;

	void bind() const;
	void unbind() const;

	GLint initializeShaders(const std::string& vertex_shader, const std::string& fragment_shader);

	static const char* base_vertex;
	static const char* base_fragment;

protected:
	GLuint m_vertex_shader;
	GLuint m_fragment_shader;
	GLuint m_program_shader;
};
