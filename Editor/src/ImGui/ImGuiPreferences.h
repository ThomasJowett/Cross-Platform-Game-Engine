#pragma once

#include "Core/Layer.h"

#include "imgui/imgui.h"

class ImGuiEditorPreferences
	:public Layer
{
public:
	explicit ImGuiEditorPreferences(bool* show);
	~ImGuiEditorPreferences() = default;
	void OnImGuiRender() override;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
	void ShowStyleEditor();
private:
	bool* m_Show;
	ImGuiStyle m_Style;
};