#pragma once

#include "core.h"

class Input
{
public: 
	inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

	inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
	inline static std::pair<double, double> GetMousePos() { return s_Instance->GetMousePosImpl(); }
	inline static double GetMouseX() { return s_Instance->GetMouseXImpl(); }
	inline static double GetMouseY() { return s_Instance->GetMouseYImpl(); }

protected:
	virtual bool IsKeyPressedImpl(int keycode) = 0;
	virtual bool IsMouseButtonPressedImpl(int button) = 0;
	virtual std::pair<double, double> GetMousePosImpl() = 0;
	virtual double GetMouseXImpl() = 0;
	virtual double GetMouseYImpl() = 0;
private:
	static Scope<Input> s_Instance;
};