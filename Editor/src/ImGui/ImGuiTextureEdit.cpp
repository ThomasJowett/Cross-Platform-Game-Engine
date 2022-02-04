#include "ImGuiTextureEdit.h"
#include "ImGui/ImGuiUtilites.h"

#include "Core/Application.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"

#include <filesystem>

IMGUI_API bool ImGui::Texture2DEdit(const char* label, Ref<Texture2D>& texture, ImVec2 size)
{
	bool edited = false;
	ImGui::Text(label);
	if (texture)
	{
		ImGui::BeginGroup();
		float textureHeight = texture->GetHeight();
		float textureWidth = texture->GetWidth();
		if(textureHeight == textureWidth)
		{
			ImGui::Image(texture, ImVec2(size.x,size.y));
		}
		else if(textureHeight < textureWidth)
		{
			float aspectRatio = textureHeight / textureWidth;
			ImGui::Dummy({0,((1.0f - aspectRatio) * size.y / 2.0f) - 0.5f * (textureWidth / size.y)});
			ImGui::Image(texture, ImVec2(size.x,aspectRatio * size.y));
			ImGui::Dummy({0,((1.0f - aspectRatio) * size.y / 2.0f) - 0.5f * (textureWidth / size.y)});
		}
		else
		{
			float aspectRatio = textureWidth / textureHeight;
			ImGui::Dummy({((1.0f - aspectRatio) * size.x / 2.0f) - 0.5f * (textureHeight / size.x), 0});
			SameLine();
			ImGui::Image(texture, ImVec2(aspectRatio * size.x, size.y));
			SameLine();
			ImGui::Dummy({((1.0f - aspectRatio) * size.x / 2.0f) - 0.5f * (textureHeight / size.x), 0});
		}
		ImGui::EndGroup();
	}
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