#include "stdafx.h"
#include "Pipeline.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLPipeline.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11Pipeline.h"
#endif // __WINDOWS__

Ref<Pipeline> Pipeline::Create(const Spec& spec)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLPipeline>(spec);
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create pipeline: DirectX is not currently supported");
		return	CreateRef<DirectX11Pipeline>(spec);
#endif // __WINDOWS__
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create pipeline: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create pipeline: Invalid Renderer API")
		return nullptr;
}