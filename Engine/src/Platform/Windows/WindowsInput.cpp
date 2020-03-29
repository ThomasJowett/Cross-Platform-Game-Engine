#include "stdafx.h"
#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "Core/Application.h"

Scope<Input> Input::s_Instance = CreateScope<WindowsInput>();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
	try
	{
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, keycode);
		return (state == GLFW_PRESS) || (state == GLFW_REPEAT);
	}
	catch (const std::bad_any_cast & e)
	{
		ENGINE_ERROR(e.what());
		return false;
	}
}

bool WindowsInput::IsMouseButtonPressedImpl(int button)
{
	try
	{
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	catch (const std::bad_any_cast & e)
	{
		ENGINE_ERROR(e.what());
		return false;
	}
}

std::pair<double, double> WindowsInput::GetMousePosImpl()
{
	try
	{
		double x, y;
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		glfwGetCursorPos(window, &x, &y);
		return { x, y };
	}
	catch (const std::bad_any_cast & e)
	{
		ENGINE_ERROR(e.what());
		return { 0.0f, 0.0f };
	}
}

double WindowsInput::GetMouseYImpl()
{
	auto [x, y] = GetMousePosImpl();
	return y;
}

double WindowsInput::GetMouseXImpl()
{
	auto [x, y] = GetMousePosImpl();
	return x;
}