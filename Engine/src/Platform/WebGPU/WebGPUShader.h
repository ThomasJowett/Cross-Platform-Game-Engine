#pragma once

#include "Renderer/Shader.h"

class WebGPUShader : public Shader
{
public:
	WebGPUShader(const std::string& name, const std::filesystem::path& fileDirectory);
	WebGPUShader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~WebGPUShader();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual std::string GetName() const override { return m_Name; }
private:
	std::unordered_map<Shader::ShaderTypes, std::string> LoadShaderSources(const std::filesystem::path& filepath);
	std::string ReadFile(const std::filesystem::path& filepath);
	void Compile(const std::unordered_map<Shader::ShaderTypes, std::string>& shaderSources);
	void CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram, const char* errorMessage);
private:
	uint32_t m_RendererID;
	std::string m_Name;
};