#pragma once

#include <vector>
#include <filesystem>

#include "Core/Layer.h"

class ProjectsStartScreen
	:public Layer
{
public:
	explicit ProjectsStartScreen(bool createProject = false);
	~ProjectsStartScreen() = default;

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;

private:
	void OpenProject(const std::filesystem::path& projectPath);
private:
	bool m_CreateProject;
	bool m_CanGoBack;

	std::vector<std::filesystem::path> m_RecentProjects;
#ifdef __EMSCRIPTEN__
	// Project folders kept by the browser, which only exist on disk until reopened
	std::vector<std::string> m_RecentWebProjects;
#endif
};