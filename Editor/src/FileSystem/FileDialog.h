#pragma once
#include "Engine.h"

#include "optional"

class FileDialog
{
public:
	// Opens a modal file dialog box and returns the file selected
	// - Filter format: "Text Files\0*.txt\0Any File\0*.*\0"
	static std::optional<std::wstring> Open(const wchar_t *title = L"Open...", const wchar_t *filter = L"Any File\0*.*\0");

	// Opens a modal file dialog box and returns a list of selected files
	// - Filter format: "Text Files\0*.txt\0Any File\0*.*\0"
	static std::optional<std::vector<std::wstring>> MultiOpen(const wchar_t *title = L"Open...", const wchar_t *filter = L"Any File\0*.*\0");

	// Opens a modal Save as file dialog and returns the name of the file that was saved
	// - Filter format: "Text Files\0*.txt\0Any File\0*.*\0"
	static std::optional<std::wstring> SaveAs(const wchar_t *title = L"Save As...", const wchar_t *filter = L"Any File\0*.*\0");
};