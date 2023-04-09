#include "stdafx.h"
#include "VulkanShader.h"

VulkanShader::VulkanShader(const std::string& name, const std::filesystem::path& fileDirectory)
{
}

VulkanShader::VulkanShader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc)
{
}

VulkanShader::~VulkanShader()
{
}

void VulkanShader::Bind() const
{
}

void VulkanShader::UnBind() const
{
}

std::string VulkanShader::GetName() const
{
    return std::string();
}
