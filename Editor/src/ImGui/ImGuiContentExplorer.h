#pragma once

#include "include.h"

class ImGuiContentExplorer
	:public Layer
{
public:
	ImGuiContentExplorer(bool* show);
	~ImGuiContentExplorer() = default;

	void OnAttach() override;
	void OnImGuiRender() override;
private:
	bool* m_Show;
	struct Internal* m_Internal;

	std::string FileDialog(const wchar_t* title, const wchar_t* filter);

	TextureLibrary2D m_TextureLibrary;
};