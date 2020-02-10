#include "stdafx.h"
#include "DirectX11Texture.h"

DirectX11Texture2D::DirectX11Texture2D(uint32_t width, uint32_t height)
{
}

DirectX11Texture2D::DirectX11Texture2D(const std::string & path)
{
}

DirectX11Texture2D::~DirectX11Texture2D()
{
}

void DirectX11Texture2D::SetData(void * data, uint32_t size)
{
}

void DirectX11Texture2D::Bind(uint32_t slot) const
{
}

std::string DirectX11Texture2D::GetName() const
{
	return m_Path;
}
