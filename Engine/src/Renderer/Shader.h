#pragma once
#include "stdafx.h"

#include "math/Matrix.h"
#include "Core/core.h"

interface Shader
{
public:
	~Shader() = default;

	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	static Ref<Shader> Create(const std::string& filepath);
	static Ref<Shader> Create(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);

	enum ShaderTypes
	{
		VERTEX = 0,
		TESS_HULL,
		TESS_DOMAIN,
		GEOMETRY,
		PIXEL,
		COMPUTE
	};
};