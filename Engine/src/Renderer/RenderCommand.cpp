#include "stdafx.h"
#include "RenderCommand.h"
#include "Core/Settings.h"
#include "Logging/Instrumentor.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#ifdef _WINDOWS
#include "Platform/DirectX/DirectX11RendererAPI.h"
#endif // _WINDOWS

Scope<RendererAPI> RenderCommand::s_RendererAPI = nullptr;

int RenderCommand::CreateRendererAPI()
{
	PROFILE_FUNCTION();
	Settings::SetDefaultValue("Renderer", "API", "OpenGL");

	std::string api = Settings::GetValue("Renderer", "API");
	if (api == "OpenGL")
	{
		RendererAPI::s_API = RendererAPI::API::OpenGL;
		s_RendererAPI = CreateScope<OpenGLRendererAPI>();
		return 0;
	}
#ifdef _WINDOWS
	else if (api == "DirectX11")
	{
		RendererAPI::s_API = RendererAPI::API::Directx11;
		s_RendererAPI = CreateScope<DirectX11RendererAPI>();
		return 0;
	}
#endif // _WINDOWS
#ifdef __APPLE__
	else if (api == "Metal")
	{
		RendererAPI::s_API = RendererAPI::API::Metal;
		CORE_ASSERT(false, "Metal not yet supported!");
		s_RendererAPI = nullptr;
		return 1;
	}
#endif // __APPLE__
	else if (api == "None")
	{
		RendererAPI::s_API = RendererAPI::API::None;
		CORE_ASSERT(false, "API of none is not currently supported!");
		s_RendererAPI = nullptr;
		return 1;
	}

	ENGINE_ERROR("API: {0} is not recognised!", api);
	return 1;
}
