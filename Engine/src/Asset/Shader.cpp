#include "stdafx.h"
#include "Shader.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#ifdef _WINDOWS
#include "Platform/DirectX/DirectX11Shader.h"
#endif // _WINDOWS

Ref<Shader> Shader::Create(const std::string& name, const std::filesystem::path& fileDirectory, bool postProcess)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLShader>(name, fileDirectory, postProcess);
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		//CORE_ASSERT(false, "Could not create Shader: DirectX is not currently supported")
		return CreateRef<DirectX11Shader>(name, fileDirectory);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Shader: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Shader: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLShader>(name, vertexShaderSrc, fragmentShaderSrc);
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		CORE_ASSERT(false, "Could not create Shader: DirectX is not currently supported")
			return CreateRef<DirectX11Shader>(name, vertexShaderSrc, fragmentShaderSrc);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Shader: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Shader: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ShaderLibrary::Add(const Ref<Shader>& shader)
{
	CORE_ASSERT(!Exists(shader->GetName()), "Shader already exists!");
	m_Shaders[shader->GetName()] = shader;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Shader> ShaderLibrary::Load(const std::string& name, bool postProcess, const std::filesystem::path& fileDirectory)
{
	if (Exists(name))
		return m_Shaders[name];

	Ref<Shader> shader = Shader::Create(name, fileDirectory, postProcess);
	Add(shader);
	return shader;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Shader> ShaderLibrary::Get(const std::string& name)
{
	CORE_ASSERT(Exists(name), "Shader does not exist!");
	return m_Shaders[name];
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool ShaderLibrary::Exists(const std::string& name) const
{
	return m_Shaders.find(name) != m_Shaders.end();
}
