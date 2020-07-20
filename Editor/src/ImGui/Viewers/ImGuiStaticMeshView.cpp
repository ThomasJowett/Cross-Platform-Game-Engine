#include "ImGuiStaticMeshView.h"

#include "IconsFontAwesome5.h"
#include "Importers/FbxImporter.h"

ImGuiStaticMeshView::ImGuiStaticMeshView(bool* show, std::filesystem::path filepath)
	:Layer("StaticMeshView"), m_Show(show), m_FilePath(filepath)
{
}

void ImGuiStaticMeshView::OnAttach()
{
	m_WindowName = ICON_FA_SHAPES + std::string(" " + m_FilePath.filename().string());

	Importer::Fbx::ImportStaticMesh(m_FilePath);
}

void ImGuiStaticMeshView::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();
}