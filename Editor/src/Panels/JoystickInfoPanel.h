#pragma once

#include "Core/Layer.h"

class JoystickInfoPanel
	:public Layer
{
public:
	explicit JoystickInfoPanel(bool* show);
	~JoystickInfoPanel() = default;
	void OnImGuiRender() override;
private:
	bool* m_Show;
};