#pragma once

#include "include.h"

class ImGuiStaticMeshView :
	public Layer
{
public:
	ImGuiStaticMeshView(bool* show, std::filesystem::path filepath);
	~ImGuiStaticMeshView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	std::string m_WindowName;

	Mesh m_Mesh;
};

