#include "stdafx.h"
#include "glfwInput.h"

#include <GLFW/glfw3.h>

#include "Core/Application.h"
#include "Core/Joysticks.h"

bool glfwInput::IsKeyPressedImpl(int keycode)
{
	try
	{
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}
	catch (const std::bad_any_cast& e)
	{
		ENGINE_ERROR(e.what());
		return false;
	}
}

bool glfwInput::IsMouseButtonPressedImpl(int button)
{
	try
	{
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	catch (const std::bad_any_cast& e)
	{
		ENGINE_ERROR(e.what());
		return false;
	}
}

std::pair<double, double> glfwInput::GetMousePosImpl()
{
	try
	{
		double x, y;
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		glfwGetCursorPos(window, &x, &y);
		return { x, y };
	}
	catch (const std::bad_any_cast& e)
	{
		ENGINE_ERROR(e.what());
		return { 0.0f, 0.0f };
	}
}

double glfwInput::GetMouseYImpl()
{
	auto [x, y] = GetMousePosImpl();
	return y;
}

double glfwInput::GetMouseXImpl()
{
	auto [x, y] = GetMousePosImpl();
	return x;
}

bool glfwInput::IsJoystickButtonPressedImpl(int joystickSlot, int button)
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

double glfwInput::GetJoystickAxisImpl(int joystickSlot, int axis)
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