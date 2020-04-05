#pragma once

#include "Core/Layer.h"

class ImGuiJoystickInfo
	:public Layer
{
public:
	ImGuiJoystickInfo(bool* show);
	~ImGuiJoystickInfo() = default;
	void OnImGuiRender() override;
private:
	bool* m_Show;
};