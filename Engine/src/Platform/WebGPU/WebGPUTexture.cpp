#include "stdafx.h"
#include "WebGPUTexture.h"
#include "Core/Application.h"
#include "Logging/Instrumentor.h"

#include <stb/stb_image.h>
#include <filesystem>

void WebGPUTexture2D::SetFilteringAndWrappingMethod()
{
	switch (m_FilterMethod)
	{
	case Texture::FilterMethod::Linear:
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case Texture::FilterMethod::Nearest:
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	default:
		break;
	}

	switch (m_WrapMethod)
	{
	case Texture::WrapMethod::Clamp:
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case Texture::WrapMethod::Mirror:
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case Texture::WrapMethod::Repeat:
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	default:
		break;
	}
}

WebGPUTexture2D::WebGPUTexture2D(uint32_t width, uint32_t height, Format format, const void* pixels)
	:m_Width(width), m_Height(height)
{
	PROFILE_FUNCTION();

	if (!pixels)
		m_Filepath = "NO DATA";

	m_Type = GL_UNSIGNED_BYTE;

	switch (format)
	{
	case Texture::Format::RED:      m_InternalFormat = GL_R8;       m_DataFormat = GL_RED;  break;
	case Texture::Format::RED8UI:   m_InternalFormat = GL_R8UI;     m_DataFormat = GL_RED;  break;
	case Texture::Format::RED16UI:  m_InternalFormat = GL_R16UI;    m_DataFormat = GL_RED;  break;
	case Texture::Format::RED32UI:  m_InternalFormat = GL_R32UI;    m_DataFormat = GL_RED;  break;
	case Texture::Format::RED32F:   m_InternalFormat = GL_R32F;     m_DataFormat = GL_RED;  break;
	case Texture::Format::RG8:      m_InternalFormat = GL_RG8;      m_DataFormat = GL_RG;   break;
	case Texture::Format::RG16F:    m_InternalFormat = GL_RG16F;    m_DataFormat = GL_RG;   m_Type = GL_FLOAT; break;
	case Texture::Format::RG32F:    m_InternalFormat = GL_RG32F;    m_DataFormat = GL_RG;   m_Type = GL_FLOAT; break;
	case Texture::Format::RGB:      m_InternalFormat = GL_RGB8;     m_DataFormat = GL_RGB;  break;
	case Texture::Format::RGBA:     m_InternalFormat = GL_RGBA8;    m_DataFormat = GL_RGBA; break;
	case Texture::Format::RGBA16F:  m_InternalFormat = GL_RGBA16F;  m_DataFormat = GL_RGBA; m_Type = GL_FLOAT; break;
	case Texture::Format::RGBA32F:  m_InternalFormat = GL_RGBA32F;  m_DataFormat = GL_RGBA; m_Type = GL_FLOAT; break;
	default: m_InternalFormat = GL_RGBA8; m_DataFormat = GL_RGBA;	break;
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
	if (pixels)
		SetData(pixels);

	SetFilteringAndWrappingMethod();
}

WebGPUTexture2D::WebGPUTexture2D(const std::filesystem::path& path)
	:m_InternalFormat(GL_FALSE), m_DataFormat(GL_FALSE), m_Height(0), m_Width(0)
{
	PROFILE_FUNCTION();

	m_Filepath = path;

	bool isValid = std::filesystem::exists(path);

	CORE_ASSERT(isValid, "Image does not exist! " + path.string());

	if (isValid){
		isValid = LoadTextureFromFile();
	} else {
		NullTexture();
	}
}

WebGPUTexture2D::~WebGPUTexture2D()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		glDeleteTextures(1, &m_RendererID);
}

void WebGPUTexture2D::SetData(const void* data)
{
	PROFILE_FUNCTION();

	m_Filepath = "";

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, m_Type, data);
}

void WebGPUTexture2D::Bind(uint32_t slot) const
{
	PROFILE_FUNCTION();
	glBindTextureUnit(slot, m_RendererID);
}

std::string WebGPUTexture2D::GetName() const
{
	return m_Filepath.filename().string();
}

uint32_t WebGPUTexture2D::GetRendererID() const
{
	return m_RendererID;
}

void WebGPUTexture2D::Reload()
{
	if (!m_Filepath.empty() || m_Filepath != "NO DATA")
	{
		glDeleteTextures(1, &m_RendererID);
		LoadTextureFromFile();
	}
}

bool WebGPUTexture2D::operator==(const Texture& other) const
{
	return m_RendererID == ((WebGPUTexture2D&)other).GetRendererID();
}

void WebGPUTexture2D::SetFilterMethod(FilterMethod filterMethod)
{
	m_FilterMethod = filterMethod;
	SetFilteringAndWrappingMethod();
}

void WebGPUTexture2D::SetWrapMethod(WrapMethod wrapMethod)
{ 
	m_WrapMethod = wrapMethod; 
	SetFilteringAndWrappingMethod();
}

void WebGPUTexture2D::NullTexture()
{
	m_Filepath = "NULL";
	m_Width = m_Height = 4;

	m_InternalFormat = GL_RGBA8, m_DataFormat = GL_RGBA;
	m_Type = GL_UNSIGNED_BYTE;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	m_FilterMethod = FilterMethod::Nearest;
	m_WrapMethod = WrapMethod::Repeat;
	SetFilteringAndWrappingMethod();

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

bool WebGPUTexture2D::LoadTextureFromFile()
{
	PROFILE_FUNCTION();

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		PROFILE_SCOPE("stbi Load Image WebGPUTexture2D(const std::string&)");
		data = stbi_load(m_Filepath.string().c_str(), &width, &height, &channels, 0);
	}

	CORE_ASSERT(data, "Failed to load image! " + m_Filepath.string());

	if (!data)
	{
		ENGINE_ERROR("stb image failure: {0}", stbi_failure_reason());
		return false;
	}

	m_Width = (uint32_t)width;
	m_Height = (uint32_t)height;

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}
	else if (channels == 1)
	{
		internalFormat = GL_R8;
		dataFormat = GL_RED;
	}

	CORE_ASSERT(internalFormat && dataFormat, "Format not supported");
	if (!(internalFormat && dataFormat))
		return false;

	m_InternalFormat = internalFormat;
	m_DataFormat = dataFormat;
	m_Type = GL_UNSIGNED_BYTE;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	SetFilteringAndWrappingMethod();

	if (m_Width % 8 == 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
	else if (m_Width % 4 == 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	else if (m_Width % 2 == 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, m_Type, data);

	stbi_image_free(data);

	return true;
}
