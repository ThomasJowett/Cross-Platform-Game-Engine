#pragma once

#include "Core/Layer.h"

#include "imgui/imgui.h"

#include "ProjectData.h"
#include "Physics/CollisionLayers.h"

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
	void ReadCollisionLayersFile();
	void SaveCollisionLayersFile();

	void DrawCollisionLayers();

	bool OnOpenDocumentChanged(Event& event) { ReadProjectFile(); ReadCollisionLayersFile(); return false; }
private:

	bool* m_Show;
	bool m_WasShown = false;
	// Whether m_ProjectData reflects a successful read of the currently open project's file -
	// see ReadProjectFile/SaveProjectFile.
	bool m_Loaded = false;

	ProjectData m_ProjectData;

	std::filesystem::path m_DefaultScenePath;

	char m_DescriptionBuffer[1024] = "";

	CollisionLayers::Names m_CollisionLayers = CollisionLayers::DefaultNames();
	bool m_CollisionLayersDirty = false;
};