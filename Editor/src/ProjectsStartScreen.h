#pragma once

#include "Engine.h"

class ProjectsStartScreen
	:public Layer
{
public:
	explicit ProjectsStartScreen();
	~ProjectsStartScreen() = default;

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;

private:
	void OpenProject(const std::filesystem::path& projectPath);
private:
	bool m_CreateProject;

	std::vector<std::filesystem::path> m_RecentProjects;
};