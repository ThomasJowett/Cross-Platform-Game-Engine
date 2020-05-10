#pragma once

#include "Core/Layer.h"

class ImGuiJoystickInfo
	:public Layer
{
public:
	explicit ImGuiJoystickInfo(bool* show);
	~ImGuiJoystickInfo() = default;
	void OnImGuiRender() override;
private:
	bool* m_Show;
};