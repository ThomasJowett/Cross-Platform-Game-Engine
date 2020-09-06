#pragma once

#include "Core/Layer.h"

#include "imgui/imgui.h"

class EditorPreferencesPanel
	:public Layer
{
public:
	explicit EditorPreferencesPanel(bool* show);
	~EditorPreferencesPanel() = default;
	void OnImGuiRender() override;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
	void ShowStyleEditor();
	bool ShowStyleSelector();
private:
	bool* m_Show;
	ImGuiStyle m_Style;

	std::string m_StyleFilename;

	bool m_VSnyc;
};