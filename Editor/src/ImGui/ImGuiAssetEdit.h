#pragma once

#include "imgui/imgui.h"

#include "Renderer/Material.h"
#include "Viewers/ViewerManager.h"
#include "FileSystem/Directory.h"

static std::filesystem::file_time_type s_AssetFileTime;

namespace ImGui
{
IMGUI_API bool MaterialEdit(const char* label, Ref<Material>& material, Ref<Material> defaultMaterial);
template<typename T>
IMGUI_API bool AssetEdit(const char* label, Ref<T>& asset, Ref<T> defaultAsset, FileType type)
{
	bool edited = false;
	ImGui::TextUnformatted(label);

	std::string assetName;
	if (asset && !asset->GetFilepath().empty())
	{
		assetName = asset->GetFilepath().filename().string();
		if (!std::filesystem::exists(asset->GetFilepath()))
		{
			asset = defaultAsset;
			assetName.clear();
		}
		else
		{
			std::filesystem::file_time_type lastWrittenTime = std::filesystem::last_write_time(asset->GetFilepath());

			if (lastWrittenTime != s_AssetFileTime)
			{
				asset->Reload();
				s_AssetFileTime = lastWrittenTime;
			}
		}
	}

	if (assetName.empty())
		assetName = "Default";

	std::string comboId = "##Combo";

	std::strcat(comboId.data(), label);

	if (ImGui::BeginCombo(comboId.c_str(), assetName.c_str()))
	{
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(type)))
		{
			if (ImGui::Selectable(file.filename().string().c_str()))
			{
				asset = AssetManager::GetAsset<T>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / file));
				edited = true;
				break;
			}
			ImGui::Tooltip(file.string().c_str());
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_AcceptPeekOnly))
		{
			std::filesystem::path* file = (std::filesystem::path*)payload->Data;

			for (std::string& ext : ViewerManager::GetExtensions(type))
			{
				if (file->extension().string() == ext)
				{
					if (ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
						asset = AssetManager::GetAsset<T>(*file);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (assetName != "Default")
	{
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PEN_TO_SQUARE"##EditAsset"))
		{
			ViewerManager::OpenViewer(asset->GetFilepath());
		}
		ImGui::Tooltip("Open Asset Viewer");

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_XMARK))
		{
			asset = defaultAsset;
		}
	}
	return edited;
}
}

