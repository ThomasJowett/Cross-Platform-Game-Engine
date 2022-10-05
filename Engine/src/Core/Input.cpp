#include "stdafx.h"
#include "Input.h"
#include "Renderer/RendererAPI.h"
#include "Platform/Windows/glfwInput.h"

Scope<Input> Input::s_Instance = nullptr;

void Input::Init()
{
	s_Instance = CreateScope<glfwInput>();
}
