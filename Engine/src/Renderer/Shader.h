#pragma once
#include "stdafx.h"

#include "math/Matrix.h"

interface Shader
{
public:
	~Shader() = default;

	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	static Shader* Create(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
};