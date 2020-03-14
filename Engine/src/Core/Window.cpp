#include "stdafx.h"
#include "Window.h"

#ifdef __WINDOWS__
#include "Platform/Windows/WindowsWindow.h"
#endif // __WINDOWS__


Scope<Window> Window::Create(const WindowProps & props)
{
#ifdef __WINDOWS__
	return CreateScope<WindowsWindow>(props);
#elif __APPLE__
	#error "Apple Not yet supported!"
#elif __linux__
	#error "Linux is not Supported"
#endif
	return nullptr;
}
