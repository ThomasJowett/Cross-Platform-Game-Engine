#include "stdafx.h"
#include "OpenGLShader.h"

#include "Core/core.h"
#include"glad/glad.h"

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

OpenGLShader::OpenGLShader(const std::string& name, const std::filesystem::path& fileDirectory)
	:m_Name(name)
{
	PROFILE_FUNCTION();
	Compile(LoadShaderSources(fileDirectory / name));
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	:m_Name(name)
{
	PROFILE_FUNCTION();
	std::unordered_map<Shader::ShaderTypes, std::string> shaderSources;

	shaderSources[Shader::ShaderTypes::VERTEX] = vertexSource;
	shaderSources[Shader::ShaderTypes::PIXEL] = fragmentSource;

	Compile(shaderSources);
}

OpenGLShader::~OpenGLShader()
{
	PROFILE_FUNCTION();
	glDeleteProgram(m_rendererID);
}

void OpenGLShader::Bind() const
{
	PROFILE_FUNCTION();
	glUseProgram(m_rendererID);
}

void OpenGLShader::UnBind() const
{
	PROFILE_FUNCTION();
	glUseProgram(0);
}

void OpenGLShader::SetMat4(const char* name, const Matrix4x4& value, bool transpose)
{
	PROFILE_FUNCTION();
	UploadUniformMat4(name, value, transpose);
}

void OpenGLShader::SetFloat4(const char* name, const float r, const float g, const float b, const float a)
{
	PROFILE_FUNCTION();
	UploadUniformFloat4(name, r, g, b, a);
}

void OpenGLShader::SetFloat4(const char* name, const Colour colour)
{
	PROFILE_FUNCTION();
	UploadUniformFloat4(name, colour.r, colour.g, colour.b, colour.a);
}

void OpenGLShader::SetInt(const char* name, const int value)
{
	PROFILE_FUNCTION();
	UploadUniformInteger(name, value);
}

void OpenGLShader::SetIntArray(const char* name, const int values[], uint32_t count)
{
	PROFILE_FUNCTION();
	UploadUniformIntegerArray(name, values, count);
}

void OpenGLShader::SetFloat(const char* name, const float value)
{
	PROFILE_FUNCTION();
	UploadUniformFloat(name, value);
}

void OpenGLShader::UploadUniformVector2(const char* name, const Vector2f& vector)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform2f(location, vector.x, vector.y);
}

void OpenGLShader::UploadUniformInteger(const char* name, const int& integer)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform1i(location, integer);
}

void OpenGLShader::UploadUniformIntegerArray(const char* name, const int* integers, uint32_t count)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform1iv(location, count, integers);
}

void OpenGLShader::UploadUniformFloat(const char* name, const float& Float)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform1f(location, Float);
}

void OpenGLShader::UploadUniformFloat4(const char* name, const float& r, const float& g, const float& b, const float& a)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniform4f(location, r, g, b, a);
}

void OpenGLShader::UploadUniformMat4(const char* name, const Matrix4x4& matrix, bool transpose)
{
	uint32_t location = glGetUniformLocation(m_rendererID, name);
	glUniformMatrix4fv(location, 1, transpose, &matrix.m[0][0]);
}

std::unordered_map<Shader::ShaderTypes, std::string> OpenGLShader::LoadShaderSources(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	std::unordered_map<Shader::ShaderTypes, std::string> shaderSources;

	std::filesystem::path shaderPath = filepath;

	shaderPath.replace_extension(".vert");

	if (std::filesystem::exists(shaderPath.replace_extension(".vert")))
	{
		shaderSources[Shader::ShaderTypes::VERTEX] = ReadFile(shaderPath);
	}

	if (std::filesystem::exists(shaderPath.replace_extension(".tesc")))
	{
		shaderSources[Shader::ShaderTypes::TESS_HULL] = ReadFile(shaderPath);
	}

	if (std::filesystem::exists(shaderPath.replace_extension(".tese")))
	{
		shaderSources[Shader::ShaderTypes::TESS_DOMAIN] = ReadFile(shaderPath);
	}

	if (std::filesystem::exists(shaderPath.replace_extension(".geom")))
	{
		shaderSources[Shader::ShaderTypes::GEOMETRY] = ReadFile(shaderPath);
	}

	if (std::filesystem::exists(shaderPath.replace_extension(".frag")))
	{
		shaderSources[Shader::ShaderTypes::PIXEL] = ReadFile(shaderPath);
	}

	if (std::filesystem::exists(shaderPath.replace_extension(".comp")))
	{
		shaderSources[Shader::ShaderTypes::COMPUTE] = ReadFile(shaderPath);
	}

	CORE_ASSERT(shaderSources.size() != 0, "No shader files found!");

	return shaderSources;
}

std::string OpenGLShader::ReadFile(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	std::string result;
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
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

void OpenGLShader::Compile(const std::unordered_map<Shader::ShaderTypes, std::string>& shaderSources)
{
	PROFILE_FUNCTION();
	GLuint program = glCreateProgram();
	CORE_ASSERT(shaderSources.size() <= 6, "There can only be 6 shaders in a shader program");

	std::array<GLenum, 6> GLShaderIDs;
	int glShaderIndex = 0;
	for (auto&& [key, value] : shaderSources)
	{
		GLenum type = ShaderTypeToGLShaderType(key);
		const std::string& source = value;

		GLuint shader = glCreateShader(type);

		// Send the shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* GLsource = (const GLchar*)source.c_str();
		glShaderSource(shader, 1, &GLsource, 0);

		// Compile the shader
		glCompileShader(shader);

		CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed: ");

		// Attach our shaders to our program
		glAttachShader(program, shader);
		GLShaderIDs[glShaderIndex++] = shader;
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
	for (int i = 0; i < shaderSources.size(); i++)
	{
		glDetachShader(m_rendererID, GLShaderIDs[i]);
		glDeleteShader(GLShaderIDs[i]);
	}
}

void OpenGLShader::CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram, const char* errorMessage)
{
	PROFILE_FUNCTION();
	GLint success = 0;
	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		char error[1024] = { 0 };
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

		ENGINE_ERROR("{0}: {1}", errorMessage, error);
	}
}