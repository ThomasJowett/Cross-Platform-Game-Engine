#include "NullTexture.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"

#include "stb/stb_image.h"

NullTexture2D::NullTexture2D(uint32_t width, uint32_t height, Format format)
	:m_Width(width), m_Height(height)
{
}

NullTexture2D::NullTexture2D(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	m_Filepath = filepath;
	if (!ReadInfoFromFile())
		SetPlaceholderSize();
}

NullTexture2D::NullTexture2D(const std::filesystem::path& filepath, const std::vector<uint8_t>& imageData)
{
	PROFILE_FUNCTION();
	int width, height, channels;
	if (stbi_info_from_memory(imageData.data(), (int)imageData.size(), &width, &height, &channels))
	{
		m_Width = width;
		m_Height = height;
		m_Channels = channels;
	}
	else
		SetPlaceholderSize();
	m_Filepath = filepath;
	m_Filepath.make_preferred();
}

bool NullTexture2D::Reload()
{
	return ReadInfoFromFile();
}

bool NullTexture2D::ReadInfoFromFile()
{
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / m_Filepath);

	int width, height, channels;
	if (!stbi_info(absolutePath.string().c_str(), &width, &height, &channels))
	{
		ENGINE_ERROR("Could not read image: {0}", absolutePath.string());
		return false;
	}

	m_Width = width;
	m_Height = height;
	m_Channels = channels;
	return true;
}

// Same size as the GPU backends' missing-texture checkerboard
void NullTexture2D::SetPlaceholderSize()
{
	m_Filepath = "NULL";
	m_Width = m_Height = 4;
	m_Channels = 4;
}
