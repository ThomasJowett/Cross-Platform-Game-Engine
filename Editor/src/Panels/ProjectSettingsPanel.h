#pragma once

#include "Core/Layer.h"

#include "ImGui/imgui.h"

#include "ProjectData.h"

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

	char m_DefaultSceneBuffer[1024] = "";
	char m_DescriptionBuffer[1024] = "";
};