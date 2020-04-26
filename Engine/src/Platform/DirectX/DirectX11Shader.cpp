#include "stdafx.h"
#include "DirectX11Shader.h"

#include <d3dcompiler.h>
#include <filesystem>

#include "Utilities/StringUtils.h"
#include "Core/Application.h"
#include "DirectX11Context.h"

const char* DirectX11Shader::shaderVersion = "vs_5_0";

DirectX11Shader::DirectX11Shader(const std::string& name, const std::string& fileDirectory)
	:m_Name(name)
{
	PROFILE_FUNCTION();
	m_ImmediateContext = dynamic_cast<DirectX11Context*>(Application::GetWindow().GetContext().get())->GetDeviceContext();

	m_VertexShader = nullptr;
	m_HullShader = nullptr;
	m_DomainShader = nullptr;
	m_GeometryShader = nullptr;
	m_PixelShader = nullptr;

	CompileShaders(fileDirectory + name + ".hlsl");
}

DirectX11Shader::DirectX11Shader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc)
{
	m_ImmediateContext = dynamic_cast<DirectX11Context*>(Application::GetWindow().GetContext().get())->GetDeviceContext();

	m_VertexShader = nullptr;
	m_HullShader = nullptr;
	m_DomainShader = nullptr;
	m_GeometryShader = nullptr;
	m_PixelShader = nullptr;
}

DirectX11Shader::~DirectX11Shader()
{
	if (m_VertexShader)		m_VertexShader->Release();
	if (m_HullShader)		m_HullShader->Release();
	if (m_DomainShader)		m_DomainShader->Release();
	if (m_GeometryShader)	m_GeometryShader->Release();
	if (m_PixelShader)		m_PixelShader->Release();
}

void DirectX11Shader::Bind() const
{
	if (m_VertexShader) m_ImmediateContext->VSSetShader(m_VertexShader, nullptr, 0);
	if (m_HullShader) m_ImmediateContext->HSSetShader(m_HullShader, nullptr, 0);
	if (m_DomainShader) m_ImmediateContext->DSSetShader(m_DomainShader, nullptr, 0);
	if (m_GeometryShader) m_ImmediateContext->GSSetShader(m_GeometryShader, nullptr, 0);
	if (m_PixelShader) m_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0);
}

void DirectX11Shader::UnBind() const
{
	if (m_VertexShader) m_ImmediateContext->VSSetShader(nullptr, nullptr, 0);
	if (m_HullShader) m_ImmediateContext->HSSetShader(nullptr, nullptr, 0);
	if (m_DomainShader) m_ImmediateContext->DSSetShader(nullptr, nullptr, 0);
	if (m_GeometryShader) m_ImmediateContext->GSSetShader(nullptr, nullptr, 0);
	if (m_PixelShader) m_ImmediateContext->PSSetShader(nullptr, nullptr, 0);
}

void DirectX11Shader::SetMat4(const char* name, const Matrix4x4& value, bool transpose)
{
}

void DirectX11Shader::SetFloat4(const char* name, const float r, const float g, const float b, const float a)
{
}

void DirectX11Shader::SetFloat4(const char* name, const Colour colour)
{
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

HRESULT DirectX11Shader::CompileShaderFromFile(WCHAR* szFilename, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif // DEBUG

	ID3DBlob* pErrorBlob = nullptr;

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

HRESULT DirectX11Shader::CompileShaders(std::string filename)
{
	if (!std::filesystem::exists(filename))
		return S_FALSE;

	HRESULT hr;
	ID3DBlob* pBlob = nullptr;

	WCHAR* wFilename = ConvertToWideChar(filename);

	ID3D11Device* pd3dDevice = dynamic_cast<DirectX11Context*>(Application::GetWindow().GetContext().get())->GetDevice();

	// Vertex Shader
	hr = CompileShaderFromFile(wFilename, "vertexShader", shaderVersion, &pBlob);
	if (SUCCEEDED(hr))
		hr = pd3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_VertexShader);

	// Hull Shader
	hr = CompileShaderFromFile(wFilename, "hullShader", shaderVersion, &pBlob);
	if (SUCCEEDED(hr))
		hr = pd3dDevice->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_HullShader);

	//Domain Shader
	hr = CompileShaderFromFile(wFilename, "domainShader", shaderVersion, &pBlob);
	if (SUCCEEDED(hr))
		hr = pd3dDevice->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_DomainShader);

	//Geometry Shader
	hr = CompileShaderFromFile(wFilename, "geometryShader", shaderVersion, &pBlob);
	if (SUCCEEDED(hr))
		hr = pd3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_GeometryShader);

	//Pixel Shader
	hr = CompileShaderFromFile(wFilename, "pixelShader", shaderVersion, &pBlob);
	if (SUCCEEDED(hr))
		hr = pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_PixelShader);

	if (FAILED(hr))
	{
		ENGINE_ERROR("Could not compile {0}", filename);
	}

	if(pBlob)	pBlob->Release();

	return hr;
}
