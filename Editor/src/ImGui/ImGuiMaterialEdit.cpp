#include "ImGuiMaterialEdit.h"

#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"

IMGUI_API bool ImGui::MaterialEdit(const char* label, Material& material)
{
	bool edited = false;
	ImGui::Text(label);

	std::string materialName;
	if (ImGui::BeginCombo("##materialEdit", materialName.c_str()))
	{
		const bool is_selected = false;
		if (ImGui::Selectable("Standard"))
		{
			material.SetShader("Standard");
		}
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::MATERIAL)))
		{
			if (ImGui::Selectable(file.filename().string().c_str(), is_selected))
			{
				material.LoadMaterial(file);
				edited = true;
				break;
			}
		}
		ImGui::EndCombo();
	}
	return edited;
}
