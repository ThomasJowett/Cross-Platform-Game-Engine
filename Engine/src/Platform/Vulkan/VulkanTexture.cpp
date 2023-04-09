#include "stdafx.h"
#include "VulkanTexture.h"

#include "Logging/Instrumentor.h"

VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height, Format format, const void* pixels)
	:m_Width(width), m_Height(height)
{
	PROFILE_FUNCTION();

	if (!pixels)
		m_Filepath = "NO DATA";

	switch (format)
	{
	case Texture::Format::RED:
		break;
	case Texture::Format::RED8UI:
		break;
	case Texture::Format::RED16UI:
		break;
	case Texture::Format::RED32UI:
		break;
	case Texture::Format::RED32F:
		break;
	case Texture::Format::RG8:
		break;
	case Texture::Format::RG16F:
		break;
	case Texture::Format::RG32F:
		break;
	case Texture::Format::RGB:
		break;
	case Texture::Format::RGBA:
		break;
	case Texture::Format::RGBA16F:
		break;
	case Texture::Format::RGBA32F:
		break;
	default:
		break;
	}
}

VulkanTexture2D::VulkanTexture2D(const std::filesystem::path& filepath)
	:m_Height(0), m_Width(0)
{
	PROFILE_FUNCTION();

	m_Filepath = filepath;

	bool isValid = std::filesystem::exists(filepath);

	CORE_ASSERT(isValid, "Image does not exist! " + filepath.string());

	if (isValid) {
		// TODO: Load texture from file
	} else {
		NullTexture();
	}
}

VulkanTexture2D::~VulkanTexture2D()
{
}

uint32_t VulkanTexture2D::GetWidth() const
{
	return 0;
}

uint32_t VulkanTexture2D::GetHeight() const
{
	return 0;
}

void VulkanTexture2D::SetData(const void* data)
{
}

void VulkanTexture2D::Bind(uint32_t slot) const
{
}

std::string VulkanTexture2D::GetName() const
{
	return std::string();
}

uint32_t VulkanTexture2D::GetRendererID() const
{
	return 0;
}

void VulkanTexture2D::Reload()
{
}

bool VulkanTexture2D::operator==(const Texture& other) const
{
	return false;
}

void VulkanTexture2D::NullTexture()
{
	m_Filepath = "NULL";
	m_Width = m_Height = 4;

	uint32_t textureData[4][4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			textureData[i][j] = ((i + j) % 2) ? 0xffff00ff : 0xff000000;
		}
	}

	SetData(&textureData);
}
