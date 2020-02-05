#include "stdafx.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11RendererAPI.h"
#endif // __WINDOWS__


Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<OpenGLRendererAPI>();

//Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope <DirectX11RendererAPI>();