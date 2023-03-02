#include "stdafx.h"
#include "Window.h"

#include "Platform/Windows/glfwWindow.h"

Scope<Window> Window::Create(const WindowProps & props)
{
	auto window = CreateScope<glfwWindow>();
	if (window->Init(props))
		return window;
	else
		return nullptr;
}
