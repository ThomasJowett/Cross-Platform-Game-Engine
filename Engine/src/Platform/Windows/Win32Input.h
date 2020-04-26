#pragma once

#include "Core/Input.h"

class Win32Input : public Input
{
protected:
	bool IsKeyPressedImpl(int keycode) override;
	bool IsMouseButtonPressedImpl(int button) override;
	std::pair<double, double> GetMousePosImpl() override;
	double GetMouseYImpl() override;
	double GetMouseXImpl() override;
	bool IsJoystickButtonPressedImpl(int joystickSlot, int button) override;
	double GetJoystickAxisImpl(int joystick, int axis) override;
};