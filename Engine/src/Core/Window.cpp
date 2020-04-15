#include "stdafx.h"
#include "Window.h"

#include "Renderer/RendererAPI.h"

#ifdef __WINDOWS__
#include "Platform/Windows/glfwWindow.h"
#include "Platform/Windows/Win32Window.h"
#endif // __WINDOWS__


Scope<Window> Window::Create(const WindowProps & props)
{
#ifdef __WINDOWS__
	if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		return CreateScope<glfwWindow>(props);
	else if (RendererAPI::GetAPI() == RendererAPI::API::Directx11)
		return CreateScope<Win32Window>(props);
#elif __APPLE__
	#error "Apple Not yet supported!"
#elif __linux__
	#error "Linux is not Supported"
#endif
	return nullptr;
}
