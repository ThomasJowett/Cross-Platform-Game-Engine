#pragma once

#include "core.h"
#include "Renderer/RendererAPI.h"
#include "MouseButtonCodes.h"
#include "KeyCodes.h"
#include "math/Vector2f.h"

struct GLFWwindow;

class Input
{
public: 
	Input(GLFWwindow* windowHandle);

	inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

	inline static bool IsMouseButtonPressed(int button) 
		{ return s_Instance->IsMouseButtonPressedImpl(button); }
	inline static bool IsMouseButtonReleased(int button)
		{ return s_Instance->m_MouseButtonsReleased.at(button); }
	inline static bool IsMouseJustPressed(int button)
		{ return s_Instance->m_MouseButtonsPressed.at(button); }
	// Returns Application's game-viewport-relative override when the Editor is Play-testing in its
	// docked viewport (see Application::SetGameViewportOverride()), or the raw OS cursor position
	// otherwise - Runtime never sets an override, so this is unaffected there.
	static std::pair<double, double> GetMousePos();
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
	// Mouse movement accumulated since the last frame (raw window cursor delta, not
	// corrected for the Editor's game-viewport scale override that GetMousePos applies).
	inline static Vector2f GetMouseDelta()
		{ return s_Instance->m_MouseDeltaAccumulator; }

	static void Init(GLFWwindow* windowHandle);

	static void SetMouseWheel(double X, double Y);
	static void SetMousePressed(int button);
	static void SetMouseReleased(int button);
	static void AccumulateMouseMotion(double xPos, double yPos);
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

	Vector2f m_MouseDeltaAccumulator;
	bool m_HasLastMousePos = false;
	double m_LastMouseX = 0.0, m_LastMouseY = 0.0;

	std::array<bool, MOUSE_BUTTON_LAST> m_MouseButtonsPressed = {};
	std::array<bool, MOUSE_BUTTON_LAST> m_MouseButtonsReleased = {};
	GLFWwindow* m_Window;
};