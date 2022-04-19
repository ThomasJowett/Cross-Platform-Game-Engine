#include "stdafx.h"
#include "OpenGLTexture.h"
#include "Core/Application.h"

#include <stb/stb_image.h>
#include <filesystem>

void OpenGLTexture2D::SetFilteringAndWrappingMethod()
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

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
	:m_Width(width), m_Height(height), m_Path("NO DATA")
{
	PROFILE_FUNCTION();

	m_InternalFormat = GL_RGBA8, m_DataFormat = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	SetFilteringAndWrappingMethod();
}

OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& path)
	:m_Path(path), m_InternalFormat(GL_FALSE), m_DataFormat(GL_FALSE), m_Height(0), m_Width(0)
{
	PROFILE_FUNCTION();

	bool isValid = std::filesystem::exists(path);

	CORE_ASSERT(isValid, "Image does not exist! " + path.string());

	if (isValid)
	{
		isValid = LoadTextureFromFile();
	}

	if (!isValid)
	{
		NullTexture();
	}
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::SetData(void* data, uint32_t size)
{
	PROFILE_FUNCTION();

	m_Path = "";

	uint32_t bytePerPixel = m_DataFormat == GL_RGBA ? 4 : 3;

	bool isValid = size == m_Width * m_Height * bytePerPixel;
	CORE_ASSERT(isValid, "Data must be entire texture");
	if (!isValid)
		NullTexture();

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
	PROFILE_FUNCTION();
	glBindTextureUnit(slot, m_RendererID);
}

std::string OpenGLTexture2D::GetName() const
{
	return m_Path.filename().string();
}

const std::filesystem::path& OpenGLTexture2D::GetFilepath() const
{
	return m_Path;
}

uint32_t OpenGLTexture2D::GetRendererID() const
{
	return m_RendererID;
}

void OpenGLTexture2D::Reload()
{
	if (!m_Path.empty() || m_Path != "NO DATA")
	{
		glDeleteTextures(1, &m_RendererID);
		LoadTextureFromFile();
	}
}

bool OpenGLTexture2D::operator==(const Texture& other) const
{
	return m_RendererID == ((OpenGLTexture2D&)other).GetRendererID();
}

void OpenGLTexture2D::NullTexture()
{
	m_Path = "NULL";
	m_Width = m_Height = 4;

	m_InternalFormat = GL_RGBA8, m_DataFormat = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

bool OpenGLTexture2D::LoadTextureFromFile()
{
	PROFILE_FUNCTION();

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		PROFILE_SCOPE("stbi Load Image OpenGLTexture2D(const std::string&)");
		data = stbi_load(m_Path.string().c_str(), &width, &height, &channels, 0);
	}

	CORE_ASSERT(data, "Failed to load image! " + m_Path.string());

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

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	return true;
}
