#include "ImGuiTextureEdit.h"
#include "ImGui/ImGuiUtilites.h"

#include "Core/Application.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"
#include "IconsFontAwesome6.h"
#include "Scene/AssetManager.h"

#include <filesystem>

bool ImGui::Texture2DEdit(const char* label, Ref<Texture2D>& texture, const ImVec2& size)
{
	bool edited = false;
	ImGui::TextUnformatted(label);
	ImGui::BeginGroup();
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
			ImGui::Dummy({ 0.0f,((1.0f - aspectRatio) * size.y / 2.0f) - 0.5f * (textureWidth / size.y) });
			ImGui::Image(texture, ImVec2(size.x, aspectRatio * size.y));
			ImGui::Dummy({ 0.0f,((1.0f - aspectRatio) * size.y / 2.0f) - 0.5f * (textureWidth / size.y) });
		}
		else
		{
			float aspectRatio = (float)textureWidth / (float)textureHeight;
			ImGui::Dummy({ ((1.0f - aspectRatio) * size.x / 2.0f) - 0.5f * ((float)textureHeight / size.x), 0.0f });
			ImGui::SameLine();
			ImGui::Image(texture, ImVec2(aspectRatio * size.x, size.y));
			ImGui::SameLine();
			ImGui::Dummy({ ((1.0f - aspectRatio) * size.x / 2.0f) - 0.5f * ((float)textureHeight / size.x), 0.0f });
		}
		ImGui::EndGroup();

		if (ImGui::IsItemHovered())
		{
			const float maxSize = size.x * 4.0f;
			ImVec2 tooltipSize;
			if (textureHeight == textureWidth && textureWidth > maxSize)
			{
				tooltipSize = ImVec2(maxSize, maxSize);
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
		ImGui::Button("##nullTexture", size);
	ImGui::SameLine();

	std::string textureName;
	if (texture)
		textureName = texture->GetName();

	ImGui::BeginGroup();
	std::string comboLabel = "##textureEdit" + std::string(label);
	if (ImGui::BeginCombo(comboLabel.c_str(), textureName.c_str()))
	{
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::IMAGE)))
		{
			const bool is_selected = false;
			if (ImGui::Selectable(file.filename().string().c_str(), is_selected))
			{
				texture = AssetManager::GetTexture(file);
				edited = true;
				break;
			}
		}
		ImGui::EndCombo();
	}

	if (texture)
	{
		ImGui::SameLine();
		if (ImGui::Button(std::string(std::string(ICON_FA_ARROW_ROTATE_LEFT"##reset") + std::string(label)).c_str()))
		{
			texture.reset();
			edited = true;
		}
		ImGui::Tooltip("Reset texture");
	}

	if (texture)
	{
		ImGui::SetNextItemWidth(150);
		Texture::FilterMethod filter = texture->GetFilterMethod();
		std::string filterlabel = "Filter Method##" + std::string(label);
		if (ImGui::Combo(filterlabel.c_str(), (int*)&filter,
			"Linear\0"
			"Nearest\0"))
		{
			texture->SetFilterMethod(filter);
			edited = true;
		}

		ImGui::Tooltip("Filter Method");

		ImGui::SetNextItemWidth(150);
		Texture::WrapMethod wrap = texture->GetWrapMethod();
		std::string wraplabel = "Wrap Method##" + std::string(label);
		if (ImGui::Combo(wraplabel.c_str(), (int*)&wrap,
			"Clamp\0"
			"Mirror\0"
			"Repeat\0"))
		{
			texture->SetWrapMethod(wrap);
			edited = true;
		}

		ImGui::Tooltip("Wrapping Method");
	}
	ImGui::EndGroup();
	ImGui::EndGroup();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_AcceptPeekOnly))
		{
			std::filesystem::path* file = (std::filesystem::path*)payload->Data;

			for (std::string& ext : ViewerManager::GetExtensions(FileType::IMAGE))
			{
				if (file->extension().string() == ext)
				{
					if (ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
						texture = AssetManager::GetTexture(*file);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	return edited;
}