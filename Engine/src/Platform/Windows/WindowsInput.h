#pragma once

#include "Core/Input.h"

class WindowsInput : public Input
{
protected:
	bool IsKeyPressedImpl(int keycode) override;
	bool IsMouseButtonPressedImpl(int button) override;
	std::pair<double, double> GetMousePosImpl() override;
	double GetMouseYImpl() override;
	double GetMouseXImpl() override;
};