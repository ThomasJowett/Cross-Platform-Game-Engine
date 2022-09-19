#include "ImGuiFileEdit.h"
#include "IconsFontAwesome6.h"
#include "FileSystem/FileDialog.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"

#include "Engine.h"

bool ImGui::FileEdit(const char* label, std::filesystem::path& filepath, const wchar_t* filter)
{
	bool edited = false;
	static char inputBuffer[1024] = "";

	memset(inputBuffer, 0, sizeof(inputBuffer));
	for (int i = 0; i < filepath.string().length(); i++)
	{
		inputBuffer[i] = filepath.string()[i];
	}

	ImGui::BeginGroup();
	ImGui::TextUnformatted(label);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 32);
	if (ImGui::InputText(("##Filepath" + std::string(label)).c_str(), inputBuffer, sizeof(inputBuffer),
		ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		filepath = inputBuffer;
		edited = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FOLDER_OPEN))
	{
		std::optional<std::wstring> dialogfilepath = FileDialog::Open(L"Open...", filter);
		if (dialogfilepath)
		{
			filepath = dialogfilepath.value();
			edited = true;
		}
	}
	ImGui::EndGroup();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
		{
			std::filesystem::path* file = (std::filesystem::path*)payload->Data;

			std::wstring filterStr = filter;

			if (filterStr.find(ConvertToWideChar(file->extension().string())) != std::string::npos)
			{
				filepath = *file;
				edited = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return edited;
}

IMGUI_API bool ImGui::FileSelect(const char* label, std::filesystem::path& filepath, FileType filetype)
{
	std::string filename = filepath.filename().string();

	bool edited = false;

	if(ImGui::BeginCombo(label, filename.c_str()))
	{
		for(std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(filetype)))
		{
			if(ImGui::Selectable(file.filename().string().c_str()))
			{
				filepath = file;
				edited = true;
			}
			ImGui::Tooltip(file.string().c_str());
		}
		ImGui::EndCombo();
	}
	if(std::filesystem::exists(filepath))
	{
		ImGui::SameLine();

		if(ImGui::Button(ICON_FA_PEN_TO_SQUARE"##editfile"))
		{
			ViewerManager::OpenViewer(filepath);
		}
	}
	return edited;
}

IMGUI_API bool ImGui::FileEdit(const char* label, std::filesystem::path& filepath, FileType filetype)
{
	const wchar_t* filter;

	switch (filetype)
	{
	case FileType::TEXT:
		break;
	case FileType::IMAGE:
		break;
	case FileType::MESH:
		filter = L"Static Mesh (.staticMesh)\0*.staticMesh\0";
		break;
	case FileType::SCENE:
		filter = L"Scene (.scene)\0*.scene\0";
		break;
	case FileType::SCRIPT:
		filter = L"Script (.cs)\0*.cs\0";
		break;
	case FileType::AUDIO:
		break;
	case FileType::MATERIAL:
		filter = L"Material (.material)\0*.material\0";
		break;
	default:
		filter = L"Any\0*.*\0";
		break;
	}
	return ImGui::FileEdit(label, filepath, filter);
}
