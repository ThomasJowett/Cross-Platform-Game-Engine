#include "stdafx.h"
#include "OpenGLShader.h"

#include "Core/core.h"
#include"GLAD/glad.h"

OpenGLShader::OpenGLShader(const std::string & vertexSource, const std::string & fragmentSource)
{
	// Create an empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Send the vertex shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	const GLchar *source = (const GLchar *)vertexSource.c_str();
	glShaderSource(vertexShader, 1, &source, 0);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(vertexShader);

		CORE_ASSERT(false, "Vertex Shader compilation faliure!");
		return;
	}

	// Create an empty fragment shader handle
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Send the fragment shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	source = (const GLchar *)fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &source, 0);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(fragmentShader);
		// Either of them. Don't leak shaders.
		glDeleteShader(vertexShader);

		CORE_ASSERT(false, "Fragment Shader compilation faliure!");
		return;
	}

	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.
	m_rendererID = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(m_rendererID, vertexShader);
	glAttachShader(m_rendererID, fragmentShader);

	// Link our program
	glLinkProgram(m_rendererID);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(m_rendererID, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_rendererID, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(m_rendererID);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		CORE_ASSERT(false, "Shader Link faliure!");
		return;
	}

	// Always detach shaders after a successful link.
	glDetachShader(m_rendererID, vertexShader);
	glDetachShader(m_rendererID, fragmentShader);
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(m_rendererID);
}

void OpenGLShader::Bind() const
{
	glUseProgram(m_rendererID);
}

void OpenGLShader::UnBind() const
{
	glUseProgram(0);
}

void OpenGLShader::UploadVector2(const char * name, const Vector2f & vector)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform2f(location, vector.x, vector.y);
}

void OpenGLShader::UploadInteger(const char * name, const int & integer)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform1i(location, integer);
}

void OpenGLShader::UploadUniformFloat4(const char * name, const float & r, const float & g, const float & b, const float & a)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform4f(location, r, g, b, a);
}

void OpenGLShader::UploadUniformMat4(const char* name, const Matrix4x4 & matrix, bool transpose)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniformMatrix4fv(location, 1, transpose, &matrix.m[0][0]);
}

