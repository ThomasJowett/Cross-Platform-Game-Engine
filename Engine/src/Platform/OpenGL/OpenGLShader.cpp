#include "stdafx.h"
#include "OpenGLShader.h"
#include "Logging/Instrumentor.h"
#include "Core/core.h"
#include <glad/glad.h>
#include "Scene/AssetManager.h"

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

OpenGLShader::OpenGLShader(const std::string& name, const std::filesystem::path& fileDirectory, bool postProcess)
	:m_Name(name)
{
	PROFILE_FUNCTION();

	std::unordered_map<Shader::ShaderTypes, std::string> shaderSources;

	if (!LoadShaderSourcesFromDisk(Application::GetWorkingDirectory() / fileDirectory / name, shaderSources))
		LoadShaderSourcesFromBundle(fileDirectory / name, shaderSources);

	if (postProcess && shaderSources.find(Shader::ShaderTypes::VERTEX) == shaderSources.end())
	{
		// load default vertex shader for post processing
		shaderSources[Shader::ShaderTypes::VERTEX] = R"(
			#version 330 core
			layout(location = 0) in vec2 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			out vec2 v_TexCoord;
			void main()
			{
				gl_Position = vec4(a_Position, 0.0, 1.0);
				v_TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
			}
		)";
	}


	CORE_ASSERT(shaderSources.size() != 0, "No shader files found!");

	Compile(shaderSources);
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
	if (Application::Get().IsRunning())
		glDeleteProgram(m_RendererID);
}

void OpenGLShader::Bind() const
{
	PROFILE_FUNCTION();
	glUseProgram(m_RendererID);
}

void OpenGLShader::UnBind() const
{
	PROFILE_FUNCTION();
	glUseProgram(0);
}

bool OpenGLShader::LoadShaderSourcesFromDisk(const std::filesystem::path& filepath, std::unordered_map<Shader::ShaderTypes, std::string>& shaderSources)
{
	PROFILE_FUNCTION();

	if (filepath.empty())
		return false;

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

	return !shaderSources.empty();
}

bool OpenGLShader::LoadShaderSourcesFromBundle(const std::filesystem::path& filepath, std::unordered_map<Shader::ShaderTypes, std::string>& shaderSources)
{
	PROFILE_FUNCTION();
	if (!AssetManager::HasBundle())
	{
		ENGINE_ERROR("No Asset Bundle loaded");
		return false;
	}
	std::vector<uint8_t> data;

	std::filesystem::path shaderPath = filepath;

	shaderPath.replace_extension(".vert");
	if (AssetManager::FileExistsInBundle(shaderPath) && AssetManager::GetFileData(shaderPath, data)) {
		std::string shaderSource(data.begin(), data.end());
		shaderSources[Shader::ShaderTypes::VERTEX] = shaderSource;
	}

	shaderPath.replace_extension(".tesc");
	if (AssetManager::FileExistsInBundle(shaderPath) && AssetManager::GetFileData(shaderPath, data)) {
		std::string shaderSource(data.begin(), data.end());
		shaderSources[Shader::ShaderTypes::TESS_HULL] = shaderSource;
	}

	shaderPath.replace_extension(".tese");
	if (AssetManager::FileExistsInBundle(shaderPath) && AssetManager::GetFileData(shaderPath, data)) {
		std::string shaderSource(data.begin(), data.end());
		shaderSources[Shader::ShaderTypes::TESS_DOMAIN] = shaderSource;
	}

	shaderPath.replace_extension(".geom");
	if (AssetManager::FileExistsInBundle(shaderPath) && AssetManager::GetFileData(shaderPath, data)) {
		std::string shaderSource(data.begin(), data.end());
		shaderSources[Shader::ShaderTypes::GEOMETRY] = shaderSource;
	}

	shaderPath.replace_extension(".frag");
	if (AssetManager::FileExistsInBundle(shaderPath) && AssetManager::GetFileData(shaderPath, data)) {
		std::string shaderSource(data.begin(), data.end());
		shaderSources[Shader::ShaderTypes::PIXEL] = shaderSource;
	}

	shaderPath.replace_extension(".comp");
	if (AssetManager::FileExistsInBundle(shaderPath) && AssetManager::GetFileData(shaderPath, data)) {
		std::string shaderSource(data.begin(), data.end());
		shaderSources[Shader::ShaderTypes::COMPUTE] = shaderSource;
	}

	return !shaderSources.empty();
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

	if (shaderSources.empty())
	{
		ENGINE_CRITICAL("Could not find required shader");
		return;
	}

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

	m_RendererID = program;

	// All the shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.

	// Link our program
	glLinkProgram(m_RendererID);
	CheckShaderError(m_RendererID, GL_LINK_STATUS, true, "ERROR: Program linking failed: ");

	glValidateProgram(m_RendererID);
	CheckShaderError(m_RendererID, GL_VALIDATE_STATUS, true, "ERROR: Program is invalid: ");

	// Always detach shaders after a successful link.
	for (int i = 0; i < shaderSources.size(); i++)
	{
		glDetachShader(m_RendererID, GLShaderIDs[i]);
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
		glDeleteProgram(m_RendererID);

		// Don't leak the shader either.
		glDeleteShader(shader);

		ENGINE_ERROR("{0}: {1}", errorMessage, error);
	}
}