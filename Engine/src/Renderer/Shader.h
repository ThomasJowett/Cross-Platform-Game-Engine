#pragma once

#include "math/Matrix.h"
#include "Core/core.h"
#include "Core/Colour.h"

#include "Core/Application.h"

#define SHADER_DIRECTORY Application::GetWorkingDirectory() / "resources" / "Shaders"

class Shader
{
public:
	~Shader() = default;

	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	virtual void SetMat4(const char* name, const Matrix4x4& value, bool transpose) = 0;
	virtual void SetFloat(const char* name, const float value) = 0;
	virtual void SetFloat2(const char* name, Vector2f value) = 0;
	virtual void SetFloat2(const char* name, const float x, const float y) = 0;
	virtual void SetFloat3(const char* name, Vector3f value) = 0;
	virtual void SetFloat3(const char* name, const float x, const float y, const float z) = 0;
	virtual void SetFloat4(const char* name, const float r, const float g, const float b, const float a) = 0;
	virtual void SetFloat4(const char* name, const Colour colour) = 0;
	virtual void SetInt(const char* name, const int value) = 0;
	virtual void SetIntArray(const char* name, const int values[], uint32_t count) = 0;

	virtual std::string GetName() const = 0;

	// Create Shader from file
	static Ref<Shader> Create(const std::string& name, const std::filesystem::path& fileDirectory = SHADER_DIRECTORY);
	//create Shader from  source strings
	static Ref<Shader> Create(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);

	enum class ShaderTypes
	{
		VERTEX = 0,
		TESS_HULL,
		TESS_DOMAIN,
		GEOMETRY,
		PIXEL,
		COMPUTE
	};
};

class ShaderLibrary
{
public:
	void Add(const Ref<Shader>& shader);
	Ref<Shader> Load(const std::string& name, const std::filesystem::path& fileDirectory = SHADER_DIRECTORY);
	Ref<Shader> Get(const std::string& name);

	bool Exists(const std::string& name) const;
private:
	std::unordered_map<std::string, Ref<Shader>> m_Shaders;
};