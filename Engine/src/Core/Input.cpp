#include "stdafx.h"
#include "Input.h"
#include "Renderer/RendererAPI.h"
#include "Platform/Windows/glfwInput.h"
#ifdef __WINDOWS__
#include "Platform/Windows/Win32Input.h"
#endif

Scope<Input> Input::s_Instance = nullptr;

void Input::SetInput(RendererAPI::API api)
{
	switch (api)
	{
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		s_Instance = CreateScope<Win32Input>();
		break;
#endif
	case RendererAPI::API::OpenGL:
		s_Instance = CreateScope<glfwInput>();
		break;
	default:
		break;
	}
}
