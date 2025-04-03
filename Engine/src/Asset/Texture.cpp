#include "stdafx.h"
#include "Texture.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/WebGPU/WebGPUTexture.h"

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, Format format, const void* pixels)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(width, height, format, pixels);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUTexture2D>(width, height, format, pixels);
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
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUTexture2D>(filepath);
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Texture: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Texture2D::Load(const std::filesystem::path& filepath)
{
	return (Texture2D::Create(filepath) == nullptr);
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

void TextureLibrary2D::Clear()
{
	m_Textures.clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool TextureLibrary2D::Exists(const std::string& name) const
{
	return m_Textures.find(name) != m_Textures.end();
}