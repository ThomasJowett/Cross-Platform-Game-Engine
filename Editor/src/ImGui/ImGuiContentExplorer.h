#pragma once

#include "include.h"

class ImGuiContentExplorer
	:public Layer
{
public:
	ImGuiContentExplorer(bool* show);
	~ImGuiContentExplorer() = default;
	void OnImGuiRender() override;
private:
	bool* m_Show;
};