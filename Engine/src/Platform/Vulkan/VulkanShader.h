#pragma once

#include "Renderer/Shader.h"

class VulkanShader : public Shader
{
public:
	VulkanShader(const std::string& name, const std::filesystem::path& fileDirectory);
	VulkanShader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~VulkanShader();

	// Inherited via Shader
	virtual void Bind() const override;
	virtual void UnBind() const override;
	virtual std::string GetName() const override;
private:
	uint32_t m_RendererID;

};
