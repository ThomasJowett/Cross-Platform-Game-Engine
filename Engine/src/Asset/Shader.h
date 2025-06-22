#pragma once

#include "math/Matrix.h"
#include "Core/core.h"
#include "Core/Colour.h"

#include "Core/Application.h"

#define SHADER_DIRECTORY "data/Shaders"

class Shader
{
public:
	virtual ~Shader() = default;

	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	virtual std::string GetName() const = 0;

	// Create Shader from file
	static Ref<Shader> Create(const std::string& name, const std::filesystem::path& fileDirectory = SHADER_DIRECTORY, bool postProcess = false);
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
	Ref<Shader> Load(const std::string& name, bool postProcess = false, const std::filesystem::path& fileDirectory = SHADER_DIRECTORY);
	Ref<Shader> Get(const std::string& name);

	bool Exists(const std::string& name) const;
private:
	std::unordered_map<std::string, Ref<Shader>> m_Shaders;
};