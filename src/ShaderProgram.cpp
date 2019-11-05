#include "ShaderProgram.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(bool is_default) :
	m_vertex_shader(0),
	m_fragment_shader(0),
	m_program_shader(0)
{}

ShaderProgram::ShaderProgram(const std::string& vertex, const std::string& fragment, bool import_from_memory) :
	ShaderProgram()
{
	if (!import_from_memory)
	{
		const std::string vertex_shader = loadShaderFromFile(vertex);
		const std::string fragment_shader = loadShaderFromFile(fragment);
		initializeShaders(vertex_shader, fragment_shader);
	}
	else
	{
		initializeShaders(vertex, fragment);
	}
}

GLint ShaderProgram::getUniformLocation(const std::string& uniform_name) const
{
	bind();
	return glGetUniformLocation(m_program_shader, uniform_name.c_str());
}

GLint ShaderProgram::getAttribLocation(const std::string& attrib_name) const
{
	bind();
	return glGetAttribLocation(m_program_shader, attrib_name.c_str());
}

void ShaderProgram::setUniform(const std::string& uniform_name, GLint value) const
{
	bind();
	GLint uniform_location = getUniformLocation(uniform_name);
	glUniform1i(uniform_location, value);
}

void ShaderProgram::setUniform(GLint uniform_location, const glm::mat4& mat) const
{
	bind();
	if (uniform_location != -1)
		glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setUniform(GLint uniform_location, float value) const
{
	bind();
	if (uniform_location != -1)
		glUniform1f(uniform_location, value);
}

void ShaderProgram::setUniform(GLint uniform_location, const glm::vec3 & value) const
{
	bind();
	if (uniform_location != -1)
		glUniform3f(uniform_location, value.x, value.y, value.z);
}

void ShaderProgram::setUniform(GLint uniform_location, const glm::vec4& value) const
{
	bind();
	if (uniform_location != -1)
		glUniform4f(uniform_location, value.x, value.y, value.z, value.w);
}

void ShaderProgram::bind() const
{
	glUseProgram(m_program_shader);
	//std::cout << "Program ID: " << m_program_shader << std::endl;
}

void ShaderProgram::unbind() const
{
	glUseProgram(0);
}

GLint ShaderProgram::initializeShaders(const std::string& vertex_shader, const std::string& fragment_shader)
{
	m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Trying to create shaders
	if (!m_vertex_shader || !m_fragment_shader)
	{
		return GL_FALSE;
	}

	// Trying to compile vertex shader
	std::cout << "Compiling vertex shader..." << std::endl;
	GLint vertex_result = createShader(vertex_shader, m_vertex_shader);
	if (vertex_result != GL_TRUE)
	{
		return vertex_result;
	}

	// Trying to compile fragment shader
	std::cout << "Compiling fragment shader..." << std::endl;
	GLint fragment_result = createShader(fragment_shader, m_fragment_shader);
	if (fragment_result != GL_TRUE)
	{
		return fragment_result;
	}

	// link the vertex and fragment shader into a program
	if (!(m_program_shader = glCreateProgram()))
	{
		return GL_FALSE;
	}
	else
	{
		GLint status;

		glAttachShader(m_program_shader, m_vertex_shader);
		glAttachShader(m_program_shader, m_fragment_shader);
		glLinkProgram(m_program_shader);
		glGetProgramiv(m_program_shader, GL_LINK_STATUS, &status);
		
		// Retrieve compilation error
		if (GL_TRUE != status)
		{
			GLint	infoLogLength;
			char*	strInfoLog;

			glGetProgramiv(m_program_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			strInfoLog = new char[infoLogLength + 1];
			glGetProgramInfoLog(m_program_shader, infoLogLength, NULL, strInfoLog);
			std::cout << strInfoLog << std::endl;
			delete[] strInfoLog;

			return GL_FALSE;
		}
	}
	
	return GL_TRUE;
}

const char* ShaderProgram::base_vertex =
"#version 430\n \
in vec2 pos;\
out vec2 texCoord;\
void main() {\
	texCoord = pos*0.5f + 0.5f;\
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\
}";

const char* ShaderProgram::base_fragment =
"#version 430\n \
uniform sampler2D srcTex;\
in vec2 texCoord;\
out vec4 color;\
void main() {\
	vec3 c = texture(srcTex, texCoord).xyz;\
	color = vec4(c, 1.0);\
}";