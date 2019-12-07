#pragma once
#include "stdafx.h"

#include "math/Matrix.h"

class Shader
{
public:
	Shader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~Shader();

	void Bind() const;
	void UnBind() const;

	void UploadUniformMat4(const char* name, const Matrix4x4& matrix, bool transpose);
private:
	uint32_t m_rendererID;
};