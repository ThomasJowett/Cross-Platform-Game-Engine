#pragma once

#include <optional>
#include <string>
#include <vector>

struct DialogFilterItem
{
	std::wstring Name;
	std::wstring Spec;
};

class FileDialog
{
public:
	// Opens a modal file dialog box and returns the file selected
	static std::optional<std::wstring> Open(const wchar_t* title = L"Open...", const std::vector<DialogFilterItem>& filters = { {L"Any File", L"*.*"} });

	// Opens a modal file dialog box and returns a list of selected files
	static std::optional<std::vector<std::wstring>> MultiOpen(const wchar_t* title = L"Open...", const std::vector<DialogFilterItem>& filters = { {L"Any File", L"*.*"} });

	// Opens a modal Save as file dialog and returns the name of the file that was saved
	static std::optional<std::wstring> SaveAs(const wchar_t* title = L"Save As...", const std::vector<DialogFilterItem>& filters = { {L"Any File", L"*.*"} });
};