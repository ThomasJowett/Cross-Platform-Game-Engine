#include "stdafx.h"
#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "Core/Application.h"

Input* Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	int state = glfwGetKey(window, keycode);
	return state == (GLFW_PRESS || GLFW_REPEAT);
}

bool WindowsInput::IsMouseButtonPressedImpl(int button)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	int state = glfwGetMouseButton(window, button);
	return state == GLFW_PRESS;
}

std::pair<double, double> WindowsInput::GetMousePosImpl()
{
	double x, y;
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	glfwGetCursorPos(window, &x, &y);
	return { x, y };
}

double WindowsInput::GetMouseYImpl()
{
	auto[x, y] = GetMousePosImpl();
	return y;
}

double WindowsInput::GetMouseXImpl()
{
	auto[x, y] = GetMousePosImpl();
	return x;
}
