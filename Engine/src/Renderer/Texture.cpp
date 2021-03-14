#include "stdafx.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11Texture.h"
#endif // __WINDOWS__

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(width, height);
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		return CreateRef<DirectX11Texture2D>(width, height);
#endif // __WINDOWS__
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Texture: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	case RendererAPI::API::Vulkan:
		CORE_ASSERT(false, "Could not create Texture: Vulkan is not currently supported")
			return nullptr;
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Texture: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Texture2D> Texture2D::Create(const std::filesystem::path& filepath)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(filepath);
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		return CreateRef<DirectX11Texture2D>(filepath);
#endif // __WINDOWS__
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Texture: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	case RendererAPI::API::Vulkan:
		CORE_ASSERT(false, "Could not create Texture: Vulkan is not currently supported")
			return nullptr;
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Texture: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void TextureLibrary2D::Add(const Ref<Texture2D>& texture)
{
	CORE_ASSERT(!Exists(texture->GetName()), "Texture already exists!");
	m_Textures[texture->GetName()] = texture;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Texture2D> TextureLibrary2D::Load(const std::filesystem::path& path)
{
	if (Exists(path.filename().string()))
		return m_Textures[path.filename().string()];

	Ref<Texture2D> texture = Texture2D::Create(path);
	Add(texture);
	return texture;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Texture2D> TextureLibrary2D::Get(const std::string& name)
{
	CORE_ASSERT(Exists(name), "Texture does not exist!");
	return m_Textures[name];
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool TextureLibrary2D::Exists(const std::string& name) const
{
	return m_Textures.find(name) != m_Textures.end();
}