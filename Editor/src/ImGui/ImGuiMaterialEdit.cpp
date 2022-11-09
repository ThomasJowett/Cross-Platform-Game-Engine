#include "ImGuiMaterialEdit.h"

#include "ImGui/ImGuiUtilites.h"
#include "Scene/AssetManager.h"

#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"
#include "IconsFontAwesome6.h"
#include "ImGuiFileEdit.h"

static std::filesystem::file_time_type s_MaterialFileTime;

IMGUI_API bool ImGui::MaterialEdit(const char* label, Ref<Material> material, Ref<Material> defaultMaterial)
{
	bool edited = false;
	ImGui::TextUnformatted(label);

	std::string materialName;
	if (material && !material->GetFilepath().empty())
	{
		materialName = material->GetFilepath().filename().string();
		if (!std::filesystem::exists(material->GetFilepath()))
		{
			material = defaultMaterial;
			materialName.clear();
		}
		else
		{
			std::filesystem::file_time_type lastWrittenTime = std::filesystem::last_write_time(material->GetFilepath());

			if (lastWrittenTime != s_MaterialFileTime)
			{
				material->Reload();
				s_MaterialFileTime = lastWrittenTime;
			}
		}
	}

	if (materialName.empty())
		materialName = "Default";

	std::string comboId = "##materialEdit";
	
	std::strcat(comboId.data(), label);

	if (ImGui::BeginCombo(comboId.c_str(), materialName.c_str()))
	{
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::MATERIAL)))
		{
			if (ImGui::Selectable(file.filename().string().c_str()))
			{
				material = AssetManager::GetMaterial(file);
				materialName = material->GetFilepath().filename().string();
				edited = true;
				break;
			}
		}
		ImGui::EndCombo();
	}

	if (materialName != "Default")
	{
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_PEN_TO_SQUARE"##EditMaterial"))
		{
			ViewerManager::OpenViewer(material->GetFilepath());
		}
		ImGui::Tooltip("Edit material");

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_XMARK))
		{
			material = defaultMaterial;
		}
	}

	return edited;
}
