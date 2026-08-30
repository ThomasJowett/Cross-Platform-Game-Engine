#include "ImGuiFileEdit.h"
#include "IconsFontAwesome6.h"
#include "FileSystem/FileDialog.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"

#include "ImGui/ImGuiUtilities.h"
#include "Utilities/StringUtils.h"
#include "Utilities/FileUtils.h"

bool ImGui::FileEdit(const char* label, std::filesystem::path& filepath, const std::vector<DialogFilterItem>& filters)
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
		std::optional<std::wstring> dialogfilepath = FileDialog::Open(L"Open...", filters);
		if (dialogfilepath)
		{
			filepath = dialogfilepath.value();
			edited = true;
		}
	}
	ImGui::EndGroup();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_AcceptPeekOnly))
		{
			std::filesystem::path* file = (std::filesystem::path*)payload->Data;

			bool match = filters.empty();
			for (const auto& filter : filters)
			{
				std::wstring spec = filter.Spec;
				if (spec.find(ConvertToWideChar(file->extension().string())) != std::string::npos || spec == L"*.*")
				{
					match = true;
					break;
				}
			}

			if (match)
			{
				if (ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
				{
					filepath = FileUtils::RelativePath(*file, Application::GetOpenDocumentDirectory());
					edited = true;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	return edited;
}

bool ImGui::FileSelect(const char* label, std::filesystem::path& filepath, FileType filetype)
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

bool ImGui::FileEdit(const char* label, std::filesystem::path& filepath, FileType filetype)
{
	std::vector<DialogFilterItem> filters;

	switch (filetype)
	{
	case FileType::TEXT:
		break;
	case FileType::IMAGE:
		break;
	case FileType::MESH:
		filters = { {L"Static Mesh (.staticMesh)", L"*.staticMesh"} };
		break;
	case FileType::SCENE:
		filters = { {L"Scene (.scene)", L"*.scene"} };
		break;
	case FileType::SCRIPT:
		filters = { {L"Script (.lua)", L"*.lua"} };
		break;
	case FileType::AUDIO:
		break;
	case FileType::MATERIAL:
		filters = { {L"Material (.material)", L"*.material"} };
		break;
	case FileType::FONT:
		filters = { {L"TrueType (.ttf)", L"*.ttf"} };
		break;
	case FileType::BEHAVIOURTREE:
		filters = { {L"Behaviour Tree (.behaviourtree)", L"*.behaviourtree"} };
		break;
	default:
		filters = { {L"Any", L"*.*"} };
		break;
	}
	return ImGui::FileEdit(label, filepath, filters);
}
