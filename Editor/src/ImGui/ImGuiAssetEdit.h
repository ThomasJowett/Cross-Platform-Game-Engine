#pragma once

#include "imgui/imgui.h"

#include "Asset/Material.h"
#include "Viewers/ViewerManager.h"
#include "FileSystem/Directory.h"

static std::filesystem::file_time_type s_AssetFileTime;

namespace ImGui
{
template<typename T>
bool AssetEdit(const char* label, Ref<T>& asset, Ref<T> defaultAsset, FileType type)
{
	bool edited = false;
	ImGui::TextUnformatted(label);

	std::string assetName;
	if (asset && !asset->GetFilepath().empty())
	{
		assetName = asset->GetFilepath().filename().string();
		auto absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / asset->GetFilepath());
		if (!std::filesystem::exists(absolutePath))
		{
			asset = defaultAsset;
			assetName.clear();
		}
		else
		{
			//std::filesystem::file_time_type lastWrittenTime = std::filesystem::last_write_time(asset->GetFilepath());
			//
			//if (lastWrittenTime != s_AssetFileTime)
			//{
			//	asset->Reload();
			//	s_AssetFileTime = lastWrittenTime;
			//}
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
				auto relativePath = FileUtils::RelativePath(file, Application::GetOpenDocumentDirectory());
				asset = AssetManager::GetAsset<T>(relativePath);
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
					{
						auto relativePath = FileUtils::RelativePath(*file, Application::GetOpenDocumentDirectory());
						asset = AssetManager::GetAsset<T>(relativePath);
					}
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
			auto absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / asset->GetFilepath());
			ViewerManager::OpenViewer(absolutePath);
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

