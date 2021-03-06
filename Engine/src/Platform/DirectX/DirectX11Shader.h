#pragma once
#include "Renderer/Shader.h"

#include <d3d11_1.h>

class DirectX11Shader :public Shader
{
public:
	DirectX11Shader(const std::string& name, const std::filesystem::path& fileDirectory);
	DirectX11Shader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~DirectX11Shader();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual void SetMat4(const char* name, const Matrix4x4& value, bool transpose) override;
	virtual void SetFloat4(const char* name, const float r, const float g, const float b, const float a) override;
	virtual void SetFloat4(const char* name, const Colour colour) override;
	virtual void SetInt(const char* name, const int value) override;
	virtual void SetIntArray(const char* name, const int values[], uint32_t count);
	virtual void SetFloat(const char* name, const float value) override;

	virtual std::string GetName()const override { return m_Name; }

	static const std::string& GetShaderVersion() { return s_ShaderVersion; }
private:
	HRESULT CompileShaderFromFile(const std::filesystem::path&, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT CompileShaders(const std::filesystem::path& filename);
private:
	std::string m_Name;

	ID3D11VertexShader* m_VertexShader;
	ID3D11HullShader* m_HullShader;
	ID3D11DomainShader* m_DomainShader;
	ID3D11GeometryShader* m_GeometryShader;
	ID3D11PixelShader* m_PixelShader;

	ID3D11ShaderReflection* m_VertexReflector;
	ID3D11ShaderReflection* m_HullReflector;
	ID3D11ShaderReflection* m_DomainReflector;
	ID3D11ShaderReflection* m_GeometryReflector;
	ID3D11ShaderReflection* m_PixelReflector;

	static std::string s_ShaderVersion;
};