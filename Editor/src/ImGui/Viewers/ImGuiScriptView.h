#pragma once
#include "include.h"

#include "ImGui/ImGuiTextEditor.h"

class ImGuiScriptView
	:public Layer
{
public:
	ImGuiScriptView(bool* show, std::filesystem::path filepath);
	~ImGuiScriptView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
private:
	TextEditor::LanguageDefinition DetermineLanguageDefinition();
private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	std::string m_WindowName;

	TextEditor m_TextEditor;
};
