#include "ImGuiStaticMeshView.h"

#include "IconsFontAwesome5.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

ImGuiStaticMeshView::ImGuiStaticMeshView(bool* show, std::filesystem::path filepath)
	:Layer("StaticMeshView"),m_Show(show), m_FilePath(filepath)
{
}

void ImGuiStaticMeshView::OnAttach()
{
	m_WindowName = ICON_FA_SHAPES + std::string(" " + m_FilePath.filename().string());


	auto asuka = aiImportFile(m_FilePath.string().c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
}

void ImGuiStaticMeshView::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(m_WindowName.c_str()), m_Show)
	{

	}
	ImGui::End();
}