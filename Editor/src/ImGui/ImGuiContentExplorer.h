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

	std::string FileDialog(const wchar_t* title, const wchar_t* filter);
};