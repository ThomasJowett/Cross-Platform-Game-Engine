#include "stdafx.h"
#include "Window.h"

#include "Renderer/RendererAPI.h"

#include "Platform/Windows/glfwWindow.h"

Scope<Window> Window::Create(const WindowProps & props)
{
	return CreateScope<glfwWindow>(props);
}
