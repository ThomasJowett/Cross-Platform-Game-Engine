#pragma once

#include "Renderer/Shader.h"

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~OpenGLShader();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	void UploadVector2(const char* name, const Vector2f& vector);
	void UploadInteger(const char* name, const int& integer);
	void UploadUniformFloat4(const char* name, const float& r, const float& g, const float& b, const float& a);
	void UploadUniformMat4(const char* name, const Matrix4x4& matrix, bool transpose);
private:
	uint32_t m_rendererID;
};