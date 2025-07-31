#pragma once

#include "Core/Layer.h"

#include "imgui/imgui.h"

#include "ProjectData.h"

#include <filesystem>

class ProjectSettingsPanel
	:public Layer
{
public:
	explicit ProjectSettingsPanel(bool* show);
	~ProjectSettingsPanel() = default;
	void OnImGuiRender() override;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& event) override;
private:
	void ReadProjectFile();
	void SaveProjectFile();

	bool OnOpenDocumentChanged(Event& event) { ReadProjectFile(); return false; }
private:

	bool* m_Show;

	ProjectData m_ProjectData;

	std::filesystem::path m_DefaultScenePath;

	char m_DescriptionBuffer[1024] = "";
};