#include "ImGuiTextureEdit.h"
#include "ImGui/ImGuiUtilites.h"

#include "Core/Application.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"

#include <filesystem>

IMGUI_API bool ImGui::Texture2DEdit(const char* label, Ref<Texture2D>& texture)
{
	bool edited = false;
	ImGui::Text(label);
	if (texture)
		ImGui::Image(texture, ImVec2(64.0f, 64.0f));
	else
		ImGui::Button("##nulltexture", ImVec2(64.0f, 64.0f));
	ImGui::SameLine();

	std::string textureName;
	if (texture)
		textureName = texture->GetName();

	if (ImGui::BeginCombo("##textureEdit", textureName.c_str()))
	{
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::IMAGE)))
		{
			const bool is_selected = false;
			if (ImGui::Selectable(file.filename().string().c_str(), is_selected))
			{
				texture = Texture2D::Create(file);
				edited = true;
				break;
			}
		}
		ImGui::EndCombo();
	}
	
	return edited;
}