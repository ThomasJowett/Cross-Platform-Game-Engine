#pragma once

#include "Renderer/Shader.h"

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~OpenGLShader();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	void UploadUniformVector2(const char* name, const Vector2f& vector);
	void UploadUniformInteger(const char* name, const int& integer);
	void UploadUniformFloat4(const char* name, const float& r, const float& g, const float& b, const float& a);
	void UploadUniformMat4(const char* name, const Matrix4x4& matrix, bool transpose);
private:
	std::unordered_map<Shader::ShaderTypes, std::string> LoadShaderSources(const std::string& shaderName);
	std::string ReadFile(const std::string& filepath);
	void Compile(const std::unordered_map<Shader::ShaderTypes, std::string> shaderSources);
	void CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram, const char* errorMessage);
private:
	uint32_t m_rendererID;

	
};