#pragma once

#include "core.h"
#include "Renderer/RendererAPI.h"

struct GLFWwindow;

class Input
{
public: 
	Input(GLFWwindow* windowHandle);

	inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

	inline static bool IsMouseButtonPressed(int button) 
		{ return s_Instance->IsMouseButtonPressedImpl(button); }
	inline static std::pair<double, double> GetMousePos() 
		{ return s_Instance->GetMousePosImpl(); }
	inline static double GetMouseX() 
		{ return s_Instance->GetMouseXImpl(); }
	inline static double GetMouseY() 
		{ return s_Instance->GetMouseYImpl(); }
	inline static bool IsJoystickButtonPressed(int joystickSlot, int button) 
		{ return s_Instance->IsJoystickButtonPressedImpl(joystickSlot, button); }
	inline static double GetJoystickAxis(int joystickSlot, int axis) 
		{ return s_Instance->GetJoystickAxisImpl(joystickSlot, axis); }
	inline static double GetMouseWheel()
		{ return s_Instance->m_MouseWheelY; }
	inline static double GetMouseWheelHorizontal()
		{ return s_Instance->m_MouseWheelX; }

	static void Init(GLFWwindow* windowHandle);

	static void SetMouseWheel(double X, double Y);
	static void SetMousePressed(int button);
	static void SetMouseReleased(int button);
	static void ClearInputData();
protected:
	virtual bool IsKeyPressedImpl(int keycode);
	virtual bool IsMouseButtonPressedImpl(int button);
	virtual std::pair<double, double> GetMousePosImpl();
	virtual double GetMouseXImpl();
	virtual double GetMouseYImpl();
	virtual bool IsJoystickButtonPressedImpl(int joystickSlot, int button);
	virtual double GetJoystickAxisImpl(int joystickSlot, int axis);
private:
	static Scope<Input> s_Instance;

	double m_MouseWheelX = 0.0f, m_MouseWheelY = 0.0f;

	std::array<bool, MOUSE_BUTTON_LAST> m_MouseButtonsPressed = {};
	std::array<bool, MOUSE_BUTTON_LAST> m_MouseButtonsReleased = {};
	GLFWwindow* m_Window;
};