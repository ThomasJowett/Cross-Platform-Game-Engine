#include "stdafx.h"
#include "Pipeline.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLPipeline.h"
#include "Platform/WebGPU/WebGPUPipeline.h"

Ref<Pipeline> Pipeline::Create(const Spec& spec)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLPipeline>(spec);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUPipeline>(spec);
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create pipeline: Invalid Renderer API")
		return nullptr;
}