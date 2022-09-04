#include "ImGuiPhysicsMaterialEdit.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"
#include "ImGui/ImGuiUtilites.h"
#include "IconsFontAwesome5.h"
#include "Scene/AssetManager.h"

static std::filesystem::file_time_type s_PhysMaterialFileTime;

IMGUI_API bool ImGui::PhysMaterialEdit(const char* label, Ref<PhysicsMaterial>& physMaterial, Ref<PhysicsMaterial>& defaultPyhsMaterial)
{
	bool edited = false;
	ImGui::Text(label);

	std::string physMaterialName;
	if (physMaterial && !physMaterial->GetFilepath().empty())
	{
		physMaterialName = physMaterial->GetFilepath().filename().string();

		std::filesystem::file_time_type lastWrittenTime = std::filesystem::last_write_time(physMaterial->GetFilepath());

		if (lastWrittenTime != s_PhysMaterialFileTime)
		{
			physMaterial->Reload();
			s_PhysMaterialFileTime = lastWrittenTime;
		}
	}

	if (physMaterialName.empty())
		physMaterialName = "Default";

	if (ImGui::BeginCombo("##PhysicsMat", physMaterialName.c_str()))
	{
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::PHYSICSMATERIAL)))
		{
			if (ImGui::Selectable(file.filename().string().c_str()))
			{
				physMaterial = AssetManager::GetPhysicsMaterial(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / file));
				edited = true;
				break;
			}
			ImGui::Tooltip(file.string().c_str());
		}
		ImGui::EndCombo();
	}

	if (physMaterialName != "Default")
	{
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PEN_SQUARE"##PhysicsMaterial"))
		{
			ViewerManager::OpenViewer(physMaterial->GetFilepath());
		}
		ImGui::Tooltip("Edit Physics Material");
	}

	return edited;
}
