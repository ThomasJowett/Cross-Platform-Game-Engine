#include "stdafx.h"
#include "Win32Input.h"
#include "Win32KeyCodes.h"

bool Win32Input::IsKeyPressedImpl(int keycode)
{
	return GetAsyncKeyState(KeyCodesToVirtualCodes(keycode));
}
bool Win32Input::IsMouseButtonPressedImpl(int button)
{
	return false;
}
std::pair<double, double> Win32Input::GetMousePosImpl()
{
	double x, y;
	return { x,y };
}

double Win32Input::GetMouseYImpl()
{
	auto [x, y] = GetMousePosImpl();
	return y;
}

double Win32Input::GetMouseXImpl()
{
	auto [x, y] = GetMousePosImpl();
	return x;
}

bool Win32Input::IsJoystickButtonPressedImpl(int joystickSlot, int button)
{
	return false;
}

double Win32Input::GetJoystickAxisImpl(int joystickSlot, int axis)
{
	return 0.0f;
}