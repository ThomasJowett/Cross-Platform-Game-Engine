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
		uint32_t textureHeight = texture->GetHeight();
		uint32_t textureWidth = texture->GetWidth();
		if (textureHeight == textureWidth)
		{
			ImGui::Image(texture, ImVec2(size.x, size.y));
		}
		else if (textureHeight < textureWidth)
		{
			float aspectRatio = (float)textureHeight / (float)textureWidth;
			ImGui::Dummy({ 0,((1.0f - aspectRatio) * size.y / 2.0f) - 0.5f * (textureWidth / size.y) });
			ImGui::Image(texture, ImVec2(size.x, aspectRatio * size.y));
			ImGui::Dummy({ 0,((1.0f - aspectRatio) * size.y / 2.0f) - 0.5f * (textureWidth / size.y) });
		}
		else
		{
			float aspectRatio = (float)textureWidth / (float)textureHeight;
			ImGui::Dummy({ ((1.0f - aspectRatio) * size.x / 2.0f) - 0.5f * ((float)textureHeight / size.x), 0 });
			SameLine();
			ImGui::Image(texture, ImVec2(aspectRatio * size.x, size.y));
			SameLine();
			ImGui::Dummy({ ((1.0f - aspectRatio) * size.x / 2.0f) - 0.5f * ((float)textureHeight / size.x), 0 });
		}
		ImGui::EndGroup();

		if (ImGui::IsItemHovered())
		{
			const uint32_t maxSize = 256;
			ImVec2 tooltipSize;
			if (textureHeight == textureWidth && textureWidth > maxSize)
			{
				tooltipSize = ImVec2(128, 128);
			}
			else if (textureHeight > textureWidth && textureHeight > maxSize)
			{
				float aspectRatio = (float)textureWidth / (float)textureHeight;
				tooltipSize = ImVec2(maxSize * aspectRatio, maxSize);
			}
			else if (textureWidth > textureHeight && textureWidth > maxSize)
			{
				float aspectRatio = (float)textureHeight / (float)textureWidth;
				tooltipSize = ImVec2(maxSize, maxSize * aspectRatio);
			}
			else
			{
				tooltipSize = ImVec2((float)textureWidth, (float)textureHeight);
			}
			ImGui::BeginTooltip();
			ImGui::Image(texture, tooltipSize);
			ImGui::EndTooltip();
		}
	}
	else
		ImGui::Button("##nulltexture", ImVec2(64.0f, 64.0f));
	ImGui::SameLine();

	std::string textureName;
	if (texture)
		textureName = texture->GetName();

	ImGui::BeginGroup();
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

	const char* filterMethodStrings[] = { "Linear", "Nearest" };
	const char* currentFilterMethodString = texture ? filterMethodStrings[(int)texture->GetFilterMethod()] : "";

	if (ImGui::BeginCombo("##Filter Method", currentFilterMethodString))
	{
		for (size_t i = 0; i < 2; i++)
		{
			bool isSelected = currentFilterMethodString == filterMethodStrings[i];

			if (ImGui::Selectable(filterMethodStrings[i], isSelected))
			{
				currentFilterMethodString = filterMethodStrings[i];
				edited = true;
				if(texture)
					texture->SetFilterMethod((Texture::FilterMethod)i);
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	ImGui::Tooltip("Filter Method");

	const char* wrapMethodStrings[] = { "Clamp", "Mirror", "Repeat" };
	const char* currentWrapMethodString = texture ? wrapMethodStrings[(int)texture->GetWrapMethod()] : "";

	if (ImGui::BeginCombo("##Wrap Method", currentWrapMethodString))
	{
		for (size_t i = 0; i < 3; i++)
		{
			bool isSelected = currentWrapMethodString == wrapMethodStrings[i];

			if (ImGui::Selectable(wrapMethodStrings[i], isSelected))
			{
				currentWrapMethodString = wrapMethodStrings[i];
				edited = true;
				if (texture)
					texture->SetWrapMethod((Texture::WrapMethod)i);
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	ImGui::Tooltip("Wrapping Method");
	ImGui::EndGroup();

	return edited;
}