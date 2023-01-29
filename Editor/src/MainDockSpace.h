#pragma once

#include "Core/Layer.h"

#include "Toolbars/PlayPauseToolbar.h"
#include "Toolbars/SaveOpenToolbar.h"
#include "ImGui/ImGuiTilemapEditor.h"

class ContentExplorerPanel;

class MainDockSpace :
	public Layer
{
public:
	MainDockSpace();
	~MainDockSpace();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& event) override;
	virtual void OnUpdate(float deltaTime) override;

	virtual void OnImGuiRender() override;

	static void SetFocussedWindow(Layer* focussedPanel) { s_CurrentlyFocusedPanel = focussedPanel; }

private:
	void OpenProject(const std::filesystem::path& filename);
	bool OnOpenProject(AppOpenDocumentChangedEvent& event);

	void HandleKeyBoardInputs();
private:
	bool m_Show;

	bool m_ShowEditorPreferences;
	bool m_ShowProjectSettings;

	bool m_ShowViewport;
	bool m_ShowConsole;
	bool m_ShowErrorList;
	bool m_ShowTaskList;
	bool m_ShowProperties;
	bool m_ShowHierarchy;
	bool m_ShowContentExplorer;
	bool m_ShowJoystickInfo;
	bool m_ShowTilemapEditor;

#ifdef DEBUG
	bool m_ShowImGuiDemo;
#endif // DEBUG


	bool m_ShowPlayPauseToolbar;
	bool m_ShowLightsToolbar;
	bool m_ShowVolumesToolbar;
	bool m_ShowLandscapeToolbar;
	bool m_ShowFoliageToolbar;
	bool m_ShowMultiplayerToolbar;
	bool m_ShowSaveOpenToolbar;
	bool m_ShowTargetPlatformToolbar;

	static Layer* s_CurrentlyFocusedPanel;

	Ref<ContentExplorerPanel> m_ContentExplorer;

	std::vector<std::filesystem::path> m_RecentProjects;

	PlayPauseToolbar m_PlayPauseToolbar;
	SaveOpenToolbar m_SaveOpenToolbar;
};