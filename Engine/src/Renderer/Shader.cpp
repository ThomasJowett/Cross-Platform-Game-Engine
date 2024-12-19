#include "stdafx.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/WebGPU/WebGPUShader.h"

Ref<Shader> Shader::Create(const std::string& name, const std::filesystem::path& fileDirectory)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLShader>(name, fileDirectory);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUShader>(name, fileDirectory);
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
		return CreateRef<OpenGLShader>(vertexShaderSrc, fragmentShaderSrc);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUShader>(vertexShaderSrc, fragmentShaderSrc);
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

Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::filesystem::path& fileDirectory)
{
	if (Exists(name))
		return m_Shaders[name];

	Ref<Shader> shader = Shader::Create(name, fileDirectory);
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
