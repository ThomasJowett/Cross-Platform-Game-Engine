#include "stdafx.h"
#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "Core/Application.h"

Scope<Input> Input::s_Instance = CreateScope<WindowsInput>();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
	int state = glfwGetKey(window, keycode);
	return (state == GLFW_PRESS) || (state == GLFW_REPEAT);
}

bool WindowsInput::IsMouseButtonPressedImpl(int button)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
	int state = glfwGetMouseButton(window, button);
	return state == GLFW_PRESS;
}

std::pair<double, double> WindowsInput::GetMousePosImpl()
{
	double x, y;
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
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
