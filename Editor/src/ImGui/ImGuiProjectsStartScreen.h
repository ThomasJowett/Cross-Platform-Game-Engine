#pragma once

#include "Core/Layer.h"

class ImGuiProjectsStartScreen
	:public Layer
{
public:
	explicit ImGuiProjectsStartScreen();
	~ImGuiProjectsStartScreen() = default;

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;
};