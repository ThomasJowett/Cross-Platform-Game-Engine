#include "stdafx.h"
#ifdef __WINDOWS__
#include "DirectX11Shader.h"

DirectX11Shader::DirectX11Shader(const std::string & name, const std::string & fileDirectory)
{
}

DirectX11Shader::DirectX11Shader(const std::string & name, const std::string & vertexShaderSrc, const std::string & fragmentShaderSrc)
{
}

DirectX11Shader::~DirectX11Shader()
{
}

void DirectX11Shader::Bind() const
{
}

void DirectX11Shader::UnBind() const
{
}

void DirectX11Shader::SetMat4(const char * name, const Matrix4x4 & value, bool transpose)
{
}

void DirectX11Shader::SetFloat4(const char * name, const float r, const float g, const float b, const float a)
{
}

void DirectX11Shader::SetFloat4(const char * name, const Colour colour)
{
}

void DirectX11Shader::SetInt(const char * name, const int value)
{
}

void DirectX11Shader::SetFloat(const char * name, const float value)
{
}
#endif