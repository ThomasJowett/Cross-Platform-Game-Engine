#include "stdafx.h"
#include "RenderCommand.h"
#include "Core/Settings.h"
#include "Logging/Instrumentor.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/WebGPU/WebGPURendererAPI.h"

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
	else if (api == "WebGPU")
	{
		RendererAPI::s_API = RendererAPI::API::WebGPU;
		s_RendererAPI = CreateScope<WebGPURendererAPI>();
		return 0;
	}
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
