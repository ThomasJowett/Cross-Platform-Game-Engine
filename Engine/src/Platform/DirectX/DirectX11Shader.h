#pragma once
#include "Renderer/Shader.h"

#include <d3d11_1.h>

class DirectX11Shader :public Shader
{
public:
	DirectX11Shader(const std::string& name, const std::string& fileDirectory = SHADER_DIRECTORY);
	DirectX11Shader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	~DirectX11Shader();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual void SetMat4(const char* name, const Matrix4x4& value, bool transpose) override;
	virtual void SetFloat4(const char* name, const float r, const float g, const float b, const float a) override;
	virtual void SetFloat4(const char * name, const Colour colour) override;
	virtual void SetInt(const char* name, const int value) override;
	virtual void SetIntArray(const char* name, const int values[], uint32_t count);
	virtual void SetFloat(const char* name, const float value) override;

	virtual std::string GetName()const override { return m_Name; }
private:
	HRESULT CompileShaderFromFile(WCHAR* szFilename, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT CompileShaders(std::string filename);
private:
	std::string m_Name;

	ID3D11DeviceContext* m_ImmediateContext;

	ID3D11VertexShader* m_VertexShader;
	ID3D11HullShader* m_HullShader;
	ID3D11DomainShader* m_DomainShader;
	ID3D11GeometryShader* m_GeometryShader;
	ID3D11PixelShader* m_PixelShader;

	static const char* shaderVersion;
};