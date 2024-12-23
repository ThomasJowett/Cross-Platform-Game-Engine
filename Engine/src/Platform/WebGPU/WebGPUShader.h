#pragma once

#include "Renderer/Shader.h"

#include <webgpu/webgpu.hpp>

class WebGPUShader : public Shader
{
public:
	WebGPUShader(const std::string& name, const std::filesystem::path& fileDirectory);
	~WebGPUShader();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual std::string GetName() const override { return m_Name; }
private:
	bool LoadShader(const std::filesystem::path& filepath);
	std::string ReadFile(const std::filesystem::path& filepath);
private:
	wgpu::ShaderModule m_Shader;
	std::string m_Name;
};