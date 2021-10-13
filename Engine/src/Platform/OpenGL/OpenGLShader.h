#pragma once

#include "Renderer/Shader.h"

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& name, const std::filesystem::path& fileDirectory);
	OpenGLShader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~OpenGLShader();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual void SetMat4(const char* name, const Matrix4x4& value, bool transpose) override;
	virtual void SetFloat2(const char* name, Vector2f value) override;
	virtual void SetFloat2(const char* name, const float x, const float y) override;
	virtual void SetFloat3(const char* name, Vector3f value) override;
	virtual void SetFloat3(const char* name, const float x, const float y, const float z) override;
	virtual void SetFloat4(const char* name, const float r, const float g, const float b, const float a) override;
	virtual void SetFloat4(const char * name, const Colour) override;
	virtual void SetInt(const char* name, const int value) override;
	virtual void SetIntArray(const char* name, const int values[], uint32_t count) override;
	virtual void SetFloat(const char* name, const float value) override;

	void UploadUniformVector2(const char* name, const Vector2f& vector);
	void UploadUniformInteger(const char* name, const int& integer);
	void UploadUniformIntegerArray(const char* name, const int* integers, uint32_t count);
	void UploadUniformFloat(const char* name, const float& Float);
	void UploadUniformFloat2(const char* name, const float& x, const float& y);
	void UploadUniformFloat3(const char* name, const float& x, const float& y, const float& z);
	void UploadUniformFloat4(const char* name, const float& r, const float& g, const float& b, const float& a);
	void UploadUniformMat4(const char* name, const Matrix4x4& matrix, bool transpose);

	virtual std::string GetName() const override { return m_Name; }
private:
	std::unordered_map<Shader::ShaderTypes, std::string> LoadShaderSources(const std::filesystem::path& filepath);
	std::string ReadFile(const std::filesystem::path& filepath);
	void Compile(const std::unordered_map<Shader::ShaderTypes, std::string>& shaderSources);
	void CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram, const char* errorMessage);
private:
	uint32_t m_rendererID;
	std::string m_Name;

	std::unordered_map<unsigned int, std::vector<uint32_t>> m_OpenGLSPIRV;
};