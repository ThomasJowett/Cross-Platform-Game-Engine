#include "ImGuiFileEdit.h"
#include "IconsFontAwesome5.h"
#include "FileSystem/FileDialog.h"

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
	ImGui::Text(label);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - 32);
	if (ImGui::InputText("##Filepath", inputBuffer, sizeof(inputBuffer),
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

			if (file->extension() == ".staticMesh")//TODO: create a filter class which has a list of extensions and names
			{
				filepath = *file;
				edited = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return edited;
}
