#include "stdafx.h"
#include "Texture.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#ifdef _WINDOWS
#include "Platform/DirectX/DirectX11Texture.h"
#endif // _WINDOWS

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, Format format, const void* pixels)
{
	PROFILE_FUNCTION();
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(width, height, format, pixels);
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		return CreateRef<DirectX11Texture2D>(width, height, format);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Texture: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Texture: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Texture2D> Texture2D::Create(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(filepath);
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		return CreateRef<DirectX11Texture2D>(filepath);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Texture: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Texture: Invalid Renderer API")
		return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::filesystem::path& filepath, const std::vector<uint8_t>& imageData)
{
	PROFILE_FUNCTION();
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(filepath, imageData);
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		return CreateRef<DirectX11Texture2D>(filepath, imageData);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Texture: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Texture: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Texture2D::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	m_Filepath = filepath;
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	return (Texture2D::Create(absolutePath) == nullptr);
}

bool Texture2D::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	m_Filepath = filepath;
	return (Texture2D::Create(filepath, data) == nullptr);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void TextureLibrary2D::Add(const Ref<Texture2D>& texture)
{
	PROFILE_FUNCTION();
	CORE_ASSERT(!Exists(texture->GetFilepath()), "Texture already exists!");
	m_Textures[texture->GetFilepath()] = texture;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Texture2D> TextureLibrary2D::Load(const std::filesystem::path& path, Ref<VirtualFileSystem> vfs)
{
	PROFILE_FUNCTION();
	if (Exists(path))
		return m_Textures[path];

	Ref<Texture2D> texture;
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / path);
	if (std::filesystem::exists(absolutePath))
	{
		texture = Texture2D::Create(path);
	}
	else if (vfs && vfs->Exists(path))
	{
		std::vector<uint8_t> data;
		AssetManager::GetFileData(path.string(), data);
		texture = Texture2D::Create(path, data);
	}

	Add(texture);
	return texture;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Texture2D> TextureLibrary2D::Get(const std::filesystem::path& name)
{
	CORE_ASSERT(Exists(name), "Texture does not exist!");
	return m_Textures[name];
}

/* ------------------------------------------------------------------------------------------------------------------ */

void TextureLibrary2D::Clear()
{
	PROFILE_FUNCTION();
	m_Textures.clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool TextureLibrary2D::Exists(const std::filesystem::path& name) const
{
	PROFILE_FUNCTION();
	return m_Textures.find(name) != m_Textures.end();
}