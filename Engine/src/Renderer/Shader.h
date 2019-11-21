#pragma once
#include "stdafx.h"

class Shader
{
public:
	Shader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~Shader();

	void Bind() const;
	void UnBind() const;
private:
	uint32_t m_rendererID;
};