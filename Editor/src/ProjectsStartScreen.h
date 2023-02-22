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
};