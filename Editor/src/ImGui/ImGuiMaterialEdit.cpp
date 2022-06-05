#include "ImGuiMaterialEdit.h"

#include "ImGui/ImGuiUtilites.h"

#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"
#include "IconsFontAwesome5.h"
#include "IconsFontAwesome6.h"
#include "ImGuiFileEdit.h"


IMGUI_API bool ImGui::MaterialEdit(const char* label, Ref<Material>& material, Ref<Material>& defaultMaterial)
{
	bool edited = false;
	ImGui::Text(label);

	std::string materialName;
	if (material)
		materialName = material->GetFilepath().filename().string();

	if (materialName.empty())
		materialName = "Default";

	if (ImGui::BeginCombo("##materialEdit", materialName.c_str()))
	{
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::MATERIAL)))
		{
			if (ImGui::Selectable(file.filename().string().c_str()))
			{
				material = CreateRef<Material>(file);
				edited = true;
				break;
			}
		}
		ImGui::EndCombo();
	}

	if (materialName != "Default")
	{
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_PEN_SQUARE"##LuaScript"))
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
