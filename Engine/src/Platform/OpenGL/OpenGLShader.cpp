#include "stdafx.h"
#include "OpenGLShader.h"

#include "Core/core.h"
#include"GLAD/glad.h"

#include <fstream>
#include <filesystem>

static GLenum ShaderTypeToGLShaderType(const Shader::ShaderTypes& type)
{
	switch (type)
	{
	case Shader::ShaderTypes::VERTEX:		return GL_VERTEX_SHADER;
	case Shader::ShaderTypes::TESS_HULL:	return GL_TESS_CONTROL_SHADER;
	case Shader::ShaderTypes::TESS_DOMAIN:	return GL_TESS_EVALUATION_SHADER;
	case Shader::ShaderTypes::GEOMETRY:		return GL_GEOMETRY_SHADER;
	case Shader::ShaderTypes::PIXEL:		return GL_FRAGMENT_SHADER;
	case Shader::ShaderTypes::COMPUTE:		return GL_COMPUTE_SHADER;

	default: 
		CORE_ASSERT(false, "UnSupported shader type")
		return 0;
	}
}

OpenGLShader::OpenGLShader(const std::string & filepath)
{
	Compile(LoadShaderSources(filepath));
}

OpenGLShader::OpenGLShader(const std::string & vertexSource, const std::string & fragmentSource)
{
	std::unordered_map<Shader::ShaderTypes, std::string> shaderSources;

	shaderSources[VERTEX] = vertexSource;
	shaderSources[PIXEL] = fragmentSource;

	Compile(shaderSources);
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

void OpenGLShader::UploadUniformVector2(const char * name, const Vector2f & vector)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform2f(location, vector.x, vector.y);
}

void OpenGLShader::UploadUniformInteger(const char * name, const int & integer)
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

std::unordered_map<Shader::ShaderTypes, std::string> OpenGLShader::LoadShaderSources(const std::string& shaderName)
{
	std::unordered_map<Shader::ShaderTypes, std::string> shaderSources;

	if (std::filesystem::exists(shaderName + ".vert"))
	{
		shaderSources[VERTEX] = ReadFile(shaderName + ".vert");
	}

	if (std::filesystem::exists(shaderName + ".tesc"))
	{
		shaderSources[TESS_HULL] = ReadFile(shaderName + ".tesc");
	}

	if (std::filesystem::exists(shaderName + ".tese"))
	{
		shaderSources[TESS_DOMAIN] = ReadFile(shaderName + ".tese");
	}

	if (std::filesystem::exists(shaderName + ".geom"))
	{
		shaderSources[GEOMETRY] = ReadFile(shaderName + ".geom");
	}

	if (std::filesystem::exists(shaderName + ".frag"))
	{
		shaderSources[PIXEL] = ReadFile(shaderName + ".frag");
	}

	if (std::filesystem::exists(shaderName + ".comp"))
	{
		shaderSources[COMPUTE] = ReadFile(shaderName + ".comp");
	}

	return shaderSources;
}

std::string OpenGLShader::ReadFile(const std::string & filepath)
{
	std::string result;
	std::ifstream file(filepath, std::ios::in, std::ios::binary);
	if (file)
	{
		file.seekg(0, std::ios::end);
		result.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&result[0], result.size());
		file.close();
	}

	return result;
}

void OpenGLShader::Compile(const std::unordered_map<Shader::ShaderTypes, std::string> shaderSources)
{
	GLuint program = glCreateProgram();
	std::vector<GLenum> GLShaderIDs;
	for (auto&& [key,value] : shaderSources)
	{
		GLenum type = ShaderTypeToGLShaderType(key);
		const std::string& source = value;

		GLuint shader = glCreateShader(type);

		// Send the shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *GLsource = (const GLchar *)source.c_str();
		glShaderSource(shader, 1, &GLsource, 0);

		// Compile the shader
		glCompileShader(shader);

		CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed: ");

		// Attach our shaders to our program
		glAttachShader(program, shader);
		GLShaderIDs.push_back(shader);
	}

	m_rendererID = program;

	// All the shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.
	
	// Link our program
	glLinkProgram(m_rendererID);
	CheckShaderError(m_rendererID, GL_LINK_STATUS, true, "ERROR: Program linking failed: ");

	glValidateProgram(m_rendererID);
	CheckShaderError(m_rendererID, GL_VALIDATE_STATUS, true, "ERROR: Program is invalid: ");

	// Always detach shaders after a successful link.
	for each (GLuint shader in GLShaderIDs)
	{
		glDetachShader(m_rendererID, shader);
	}
}

void OpenGLShader::CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram, const char * errorMessage)
{
	GLint success = 0;
	char error[1024] = { 0 };
	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
		{
			glGetProgramInfoLog(shader, sizeof(error), nullptr, error);
		}
		else
			glGetShaderInfoLog(shader, sizeof(error), nullptr, error);

		// We don't need the program anymore.
		glDeleteProgram(m_rendererID);

		// Don't leak the shader either.
		glDeleteShader(shader);

		std::cerr << errorMessage << ": " << error << " " << std::endl;

		CORE_ASSERT(false, "Shader Error!");
	}
}

