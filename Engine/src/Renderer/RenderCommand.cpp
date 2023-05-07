#include "stdafx.h"
#include "RenderCommand.h"
#include "Core/Settings.h"
#include "Logging/Instrumentor.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11RendererAPI.h"
#endif // __WINDOWS__
#ifdef HAS_VULKAN_SDK
#include "Platform/Vulkan/VulkanRendererAPI.h"
#endif // HAS_VULKAN_SDK

Scope<RendererAPI> RenderCommand::s_RendererAPI = nullptr;

void RenderCommand::CreateRendererAPI()
{
	PROFILE_FUNCTION();
	Settings::SetDefaultValue("Renderer", "API", "OpenGL");

	std::string api = Settings::GetValue("Renderer", "API");
	if (api == "OpenGL")
	{
		RendererAPI::s_API = RendererAPI::API::OpenGL;
		s_RendererAPI = CreateScope<OpenGLRendererAPI>();
		return;
	}
#ifdef __WINDOWS__
	else if(api == "DirectX11")
	{
		RendererAPI::s_API = RendererAPI::API::Directx11;
		s_RendererAPI = CreateScope<DirectX11RendererAPI>();
		return;
	}
#endif // __WINDOWS__
#ifdef __APPLE__
	else if (api == "Metal")
	{
		RendererAPI::s_API = RendererAPI::API::Metal;
		CORE_ASSERT(false, "Metal not yet supported!");
		s_RendererAPI = nullptr;
		return;
	}
#endif // __APPLE__
#ifdef HAS_VULKAN_SDK
	else if (api == "Vulkan")
	{
		RendererAPI::s_API = RendererAPI::API::Vulkan;
		s_RendererAPI = CreateScope<VulkanRendererAPI>();
		return;
	}
#endif
	else if (api == "None")
	{
		RendererAPI::s_API = RendererAPI::API::None;
		CORE_ASSERT(false, "API of none is not currently supported!");
		s_RendererAPI = nullptr;
	}

	ENGINE_ERROR("API: {0} is not recognised!", api);
}
