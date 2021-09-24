#include "stdafx.h"
#include "DirectX11Shader.h"

#include <filesystem>

#include "Utilities/StringUtils.h"
#include "Core/Application.h"
#include "DirectX11Context.h"

#pragma comment(lib, "dxguid.lib")

std::string DirectX11Shader::s_ShaderVersion = "_5_0";

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;

DirectX11Shader::DirectX11Shader(const std::string& name, const std::filesystem::path& fileDirectory)
	:m_Name(name)
{
	PROFILE_FUNCTION();

	m_VertexShader = nullptr;
	m_HullShader = nullptr;
	m_DomainShader = nullptr;
	m_GeometryShader = nullptr;
	m_PixelShader = nullptr;

	m_VertexReflector = nullptr;
	m_HullReflector = nullptr;
	m_DomainReflector = nullptr;
	m_GeometryReflector = nullptr;
	m_PixelReflector = nullptr;

	std::filesystem::path shaderPath = fileDirectory / name;
	shaderPath.replace_extension(".hlsl");

	CompileShaders(shaderPath);
}

DirectX11Shader::DirectX11Shader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc)
{
	m_VertexShader = nullptr;
	m_HullShader = nullptr;
	m_DomainShader = nullptr;
	m_GeometryShader = nullptr;
	m_PixelShader = nullptr;

	m_VertexReflector = nullptr;
	m_HullReflector = nullptr;
	m_DomainReflector = nullptr;
	m_GeometryReflector = nullptr;
	m_PixelReflector = nullptr;
}

DirectX11Shader::~DirectX11Shader()
{
	if (m_VertexShader)		m_VertexShader->Release();
	if (m_HullShader)		m_HullShader->Release();
	if (m_DomainShader)		m_DomainShader->Release();
	if (m_GeometryShader)	m_GeometryShader->Release();
	if (m_PixelShader)		m_PixelShader->Release();

	if (m_VertexReflector)		m_VertexReflector->Release();
	if (m_HullReflector)		m_HullReflector->Release();
	if (m_DomainReflector)		m_DomainReflector->Release();
	if (m_GeometryReflector)	m_GeometryReflector->Release();
	if (m_PixelReflector)		m_PixelReflector->Release();
}

void DirectX11Shader::Bind() const
{
	if (m_VertexShader) g_ImmediateContext->VSSetShader(m_VertexShader, nullptr, 0);
	if (m_HullShader) g_ImmediateContext->HSSetShader(m_HullShader, nullptr, 0);
	if (m_DomainShader) g_ImmediateContext->DSSetShader(m_DomainShader, nullptr, 0);
	if (m_GeometryShader) g_ImmediateContext->GSSetShader(m_GeometryShader, nullptr, 0);
	if (m_PixelShader) g_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0);
}

void DirectX11Shader::UnBind() const
{
	if (m_VertexShader) g_ImmediateContext->VSSetShader(nullptr, nullptr, 0);
	if (m_HullShader) g_ImmediateContext->HSSetShader(nullptr, nullptr, 0);
	if (m_DomainShader) g_ImmediateContext->DSSetShader(nullptr, nullptr, 0);
	if (m_GeometryShader) g_ImmediateContext->GSSetShader(nullptr, nullptr, 0);
	if (m_PixelShader) g_ImmediateContext->PSSetShader(nullptr, nullptr, 0);
}

void DirectX11Shader::SetMat4(const char* name, const Matrix4x4& value, bool transpose)
{
}

void DirectX11Shader::SetFloat2(const char* name, Vector2f value)
{
}

void DirectX11Shader::SetFloat2(const char* name, const float x, const float y)
{
}

void DirectX11Shader::SetFloat3(const char* name, Vector3f value)
{
}

void DirectX11Shader::SetFloat3(const char* name, const float x, const float y, const float z)
{
}

void DirectX11Shader::SetFloat4(const char* name, const float r, const float g, const float b, const float a)
{
}

void DirectX11Shader::SetFloat4(const char* name, const Colour colour)
{
	D3D11_SHADER_INPUT_BIND_DESC shaderInputBindingDesc;
	if (m_VertexShader) m_VertexReflector->GetResourceBindingDescByName(name, &shaderInputBindingDesc);
}

void DirectX11Shader::SetInt(const char* name, const int value)
{
}

void DirectX11Shader::SetIntArray(const char* name, const int values[], uint32_t count)
{
}

void DirectX11Shader::SetFloat(const char* name, const float value)
{
}

HRESULT DirectX11Shader::CompileShaderFromFile(const std::filesystem::path& filename, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif // DEBUG

	ID3DBlob* pErrorBlob = nullptr;

	LPCWSTR szFilename = ConvertToWideChar(filename.string());

	hr = D3DCompileFromFile(szFilename, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
		{
			ENGINE_ERROR((char*)pErrorBlob->GetBufferPointer());
		}

		ENGINE_ERROR("Cannot compile {0}", szEntryPoint);
	}

	if (pErrorBlob) pErrorBlob->Release();

	return hr;
}

HRESULT DirectX11Shader::CompileShaders(const std::filesystem::path& filename)
{
	if (!std::filesystem::exists(filename))
	{
		ENGINE_ERROR("{0} shader file does not exist", filename.string());
		return S_FALSE;
	}
	HRESULT hr;
	ID3DBlob* pBlob = nullptr;

	// Vertex Shader
	hr = CompileShaderFromFile(filename, "vertexShader", ("vs" + s_ShaderVersion).c_str(), &pBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_D3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_VertexShader);

		if (SUCCEEDED(hr))
			D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_VertexReflector);
	}

	// Hull Shader
	hr = CompileShaderFromFile(filename, "hullShader", ("hs" + s_ShaderVersion).c_str(), &pBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_D3dDevice->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_HullShader);
		D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_HullReflector);
	}

	//Domain Shader
	hr = CompileShaderFromFile(filename, "domainShader", ("ds" + s_ShaderVersion).c_str(), &pBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_D3dDevice->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_DomainShader);


		if (SUCCEEDED(hr))
			D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_DomainReflector);
	}

	//Geometry Shader
	hr = CompileShaderFromFile(filename, "geometryShader", ("gs" + s_ShaderVersion).c_str(), &pBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_D3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_GeometryShader);

		if (SUCCEEDED(hr))
			D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_GeometryReflector);
	}

	//Pixel Shader
	hr = CompileShaderFromFile(filename, "pixelShader", ("ps" + s_ShaderVersion).c_str(), &pBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_D3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_PixelShader);

		if (SUCCEEDED(hr))
			D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_PixelReflector);
	}

	if (FAILED(hr))
	{
		ENGINE_ERROR("Could not compile {0}", filename);
	}

	if (pBlob)	pBlob->Release();

	return hr;
}
