#include "stdafx.h"
#include "DirectX11Texture.h"
#include "DirectX11Context.h"
#include "Core/Application.h"

#include <stb/stb_image.h>

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;

DirectX11Texture2D::DirectX11Texture2D(uint32_t width, uint32_t height)
	:m_Width(width), m_Height(height)
{
	m_Path = "NO DATA";

	m_ShaderResourceView = nullptr;
}

DirectX11Texture2D::DirectX11Texture2D(const std::string & path)
	:m_Path(path)
{
	PROFILE_FUNCTION();

	m_ShaderResourceView = nullptr;

	bool isValid = std::filesystem::exists(path);

	CORE_ASSERT(isValid, "Image does not exist! " + path);

	if (isValid)
	{
		isValid = LoadTextureFromFile();
	}

	if (!isValid)
	{
		NullTexture();
	}
}

DirectX11Texture2D::~DirectX11Texture2D()
{
	if(m_ShaderResourceView) m_ShaderResourceView->Release();
}

void DirectX11Texture2D::SetData(void * data, uint32_t size)
{
	m_Path = "";
}

void DirectX11Texture2D::Bind(uint32_t slot) const
{
	g_ImmediateContext->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
}

std::string DirectX11Texture2D::GetName() const
{
	return m_Path;
}

uint32_t DirectX11Texture2D::GetRendererID() const
{
	return uint32_t(); // TODO: return renderer
}

bool DirectX11Texture2D::operator==(const Texture& other) const
{
	return false; // TODO: compare rendererIDs
}

void DirectX11Texture2D::NullTexture()
{
	m_Path = "NULL";
	m_Width = m_Height = 4;

	uint32_t textureData[4][4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			textureData[i][j] = ((i + j) % 2) ? 0xffff00ff : 0xff000000;
		}
	}

	SetData(&textureData, sizeof(uint32_t) * 4 * 4);
}

bool DirectX11Texture2D::LoadTextureFromFile()
{
	int width, height, channels;
	unsigned char* data = nullptr;
	{
		PROFILE_SCOPE("stbi Load Image Directx11Texture2D(const std::string&)");
		data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
	}

	CORE_ASSERT(data, "Failed to load image: " + m_Path);

	if (!data)
	{
		ENGINE_ERROR("stb image failure: {0}", stbi_failure_reason());
		return false;
	}

	m_Width = (uint32_t)width;
	m_Height = (uint32_t)height;

	DXGI_FORMAT internalFormat = DXGI_FORMAT_UNKNOWN;

	if (channels == 4)
	{
		internalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else if (channels == 3)
	{
		internalFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
	}
	else if (channels == 1)
	{
		internalFormat = DXGI_FORMAT_R8_UNORM;
	}

	//Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = internalFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = data;
	subresource.SysMemPitch = desc.Width;
	subresource.SysMemSlicePitch = 0;
	g_D3dDevice->CreateTexture2D(&desc, &subresource, &pTexture);

	//Create Shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = internalFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_D3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &m_ShaderResourceView);
	pTexture->Release();

	stbi_image_free(data);
	return true;
}
