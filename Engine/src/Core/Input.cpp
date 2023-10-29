#include "stdafx.h"
#include "Input.h"
#include "Renderer/RendererAPI.h"

#include <GLFW/glfw3.h>

#include "Core/Joysticks.h"

Scope<Input> Input::s_Instance = nullptr;

Input::Input(GLFWwindow* windowHandle)
	:m_Window(windowHandle)
{
}

void Input::Init(GLFWwindow* windowHandle)
{
	s_Instance = CreateScope<Input>(windowHandle);
}

void Input::SetMouseWheel(double X, double Y)
{
	s_Instance->m_MouseWheelX += X; 
	s_Instance->m_MouseWheelY += Y;
};

bool Input::IsKeyPressedImpl(int keycode)
{
	try
	{
		int state = glfwGetKey(m_Window, keycode);
		return state == GLFW_PRESS;
	}
	catch (const std::bad_any_cast& e)
	{
		ENGINE_ERROR(e.what());
		return false;
	}
}

bool Input::IsMouseButtonPressedImpl(int button)
{
	try
	{
		int state = glfwGetMouseButton(m_Window, button);
		return state == GLFW_PRESS;
	}
	catch (const std::bad_any_cast& e)
	{
		ENGINE_ERROR(e.what());
		return false;
	}
}

std::pair<double, double> Input::GetMousePosImpl()
{
	try
	{
		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);
		return { x, y };
	}
	catch (const std::bad_any_cast& e)
	{
		ENGINE_ERROR(e.what());
		return { 0.0f, 0.0f };
	}
}

double Input::GetMouseYImpl()
{
	auto [x, y] = GetMousePosImpl();
	return y;
}

double Input::GetMouseXImpl()
{
	auto [x, y] = GetMousePosImpl();
	return x;
}

bool Input::IsJoystickButtonPressedImpl(int joystickSlot, int button)
{
	Joysticks::Joystick joystick = Joysticks::GetJoystick(joystickSlot);
	if (joystick.isMapped)
	{
		GLFWgamepadstate state;

		if (glfwGetGamepadState(Joysticks::GetJoystick(joystickSlot).id, &state) && button <= GLFW_GAMEPAD_BUTTON_LAST)
		{
			return state.buttons[button] == GLFW_PRESS;
		}
	}
	else
	{
		int button_count;
		const unsigned char* buttons = glfwGetJoystickButtons(joystickSlot, &button_count);

		if (button < button_count)
			return buttons[button];
	}
	return false;
}

double Input::GetJoystickAxisImpl(int joystickSlot, int axis)
{
	GLFWgamepadstate state;

	if (glfwGetGamepadState(Joysticks::GetJoystick(joystickSlot).id, &state) && axis <= GLFW_GAMEPAD_AXIS_LAST)
	{
		if (state.axes[axis] >= Joysticks::GetDeadZone() || state.axes[axis] <= -Joysticks::GetDeadZone())
			return state.axes[axis];
	}

	int axes_count;
	const float* axes = glfwGetJoystickAxes(joystickSlot, &axes_count);

	if (axis < axes_count)
		return axes[axis];

	return 0.0f;
}