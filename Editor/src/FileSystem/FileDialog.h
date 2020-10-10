#pragma once
#include "Engine.h"

// Opens a modal file dialog box and returns the file selected 
// - Filter format: "Text Files\0*.txt\0Any File\0*.*\0"
static std::wstring FileDialog(const wchar_t* title = L"Open...", const wchar_t* filter = L"Any File\0*.*\0")
{
#ifdef __WINDOWS__
	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:		ENGINE_ERROR("CDERR_DIALOGFAILURE");   break;
		case CDERR_FINDRESFAILURE:		ENGINE_ERROR("CDERR_FINDRESFAILURE");  break;
		case CDERR_INITIALIZATION:		ENGINE_ERROR("CDERR_INITIALIZATION");  break;
		case CDERR_LOADRESFAILURE:		ENGINE_ERROR("CDERR_LOADRESFAILURE");  break;
		case CDERR_LOADSTRFAILURE:		ENGINE_ERROR("CDERR_LOADSTRFAILURE");  break;
		case CDERR_LOCKRESFAILURE:		ENGINE_ERROR("CDERR_LOCKRESFAILURE");  break;
		case CDERR_MEMALLOCFAILURE:		ENGINE_ERROR("CDERR_MEMALLOCFAILURE"); break;
		case CDERR_MEMLOCKFAILURE:		ENGINE_ERROR("CDERR_MEMLOCKFAILURE");  break;
		case CDERR_NOHINSTANCE:			ENGINE_ERROR("CDERR_NOHINSTANCE");     break;
		case CDERR_NOHOOK:				ENGINE_ERROR("CDERR_NOHOOK");          break;
		case CDERR_NOTEMPLATE:			ENGINE_ERROR("CDERR_NOTEMPLATE");      break;
		case CDERR_STRUCTSIZE:			ENGINE_ERROR("CDERR_STRUCTSIZE");      break;
		case FNERR_BUFFERTOOSMALL:		ENGINE_ERROR("FNERR_BUFFERTOOSMALL");  break;
		case FNERR_INVALIDFILENAME:		ENGINE_ERROR("FNERR_INVALIDFILENAME"); break;
		case FNERR_SUBCLASSFAILURE:		ENGINE_ERROR("FNERR_SUBCLASSFAILURE"); break;
		default: ENGINE_INFO("File open cancelled.");
		}
	}

	return std::wstring(filename);
	//return std::string(ws.begin(), ws.end());
#endif // __WINDOWS__

	return std::wstring();
}

// Opens a modal file dialog box and returns a list of selected files 
// - Filter format: "Text Files\0*.txt\0Any File\0*.*\0"
static std::vector<std::string> MultiFileDialog(const wchar_t* title = L"Open...", const wchar_t* filter = L"Any File\0*.*\0")
{
#ifdef __WINDOWS__
	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

	if (!GetOpenFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:		ENGINE_ERROR("CDERR_DIALOGFAILURE");   break;
		case CDERR_FINDRESFAILURE:		ENGINE_ERROR("CDERR_FINDRESFAILURE");  break;
		case CDERR_INITIALIZATION:		ENGINE_ERROR("CDERR_INITIALIZATION");  break;
		case CDERR_LOADRESFAILURE:		ENGINE_ERROR("CDERR_LOADRESFAILURE");  break;
		case CDERR_LOADSTRFAILURE:		ENGINE_ERROR("CDERR_LOADSTRFAILURE");  break;
		case CDERR_LOCKRESFAILURE:		ENGINE_ERROR("CDERR_LOCKRESFAILURE");  break;
		case CDERR_MEMALLOCFAILURE:		ENGINE_ERROR("CDERR_MEMALLOCFAILURE"); break;
		case CDERR_MEMLOCKFAILURE:		ENGINE_ERROR("CDERR_MEMLOCKFAILURE");  break;
		case CDERR_NOHINSTANCE:			ENGINE_ERROR("CDERR_NOHINSTANCE");     break;
		case CDERR_NOHOOK:				ENGINE_ERROR("CDERR_NOHOOK");          break;
		case CDERR_NOTEMPLATE:			ENGINE_ERROR("CDERR_NOTEMPLATE");      break;
		case CDERR_STRUCTSIZE:			ENGINE_ERROR("CDERR_STRUCTSIZE");      break;
		case FNERR_BUFFERTOOSMALL:		ENGINE_ERROR("FNERR_BUFFERTOOSMALL");  break;
		case FNERR_INVALIDFILENAME:		ENGINE_ERROR("FNERR_INVALIDFILENAME"); break;
		case FNERR_SUBCLASSFAILURE:		ENGINE_ERROR("FNERR_SUBCLASSFAILURE"); break;
		default: ENGINE_INFO("File open cancelled.");
		}
	}

	std::vector<std::string> files;

	wchar_t* str = ofn.lpstrFile;
	std::wstring directory = str;
	str += (directory.length() + 1);
	if (!*str)
	{
		files.push_back(std::string(directory.begin(), directory.end()));
		return files;
	}
	while (*str) {
		std::wstring filename = str;
		str += (filename.length() + 1);
		std::wstring ws(filename);
		files.push_back(std::string(directory.begin(), directory.end()) + std::string(ws.begin(), ws.end()));
	}

	return files;
#endif // __WINDOWS__

	return std::vector<std::string>();
}

static std::wstring SaveAsDialog(const wchar_t* title = L"Save As...", const wchar_t* filter = L"Any File\0*.*\0")
{
#ifdef __WINDOWS__

	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (!GetSaveFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:		ENGINE_ERROR("CDERR_DIALOGFAILURE");   break;
		case CDERR_FINDRESFAILURE:		ENGINE_ERROR("CDERR_FINDRESFAILURE");  break;
		case CDERR_INITIALIZATION:		ENGINE_ERROR("CDERR_INITIALIZATION");  break;
		case CDERR_LOADRESFAILURE:		ENGINE_ERROR("CDERR_LOADRESFAILURE");  break;
		case CDERR_LOADSTRFAILURE:		ENGINE_ERROR("CDERR_LOADSTRFAILURE");  break;
		case CDERR_LOCKRESFAILURE:		ENGINE_ERROR("CDERR_LOCKRESFAILURE");  break;
		case CDERR_MEMALLOCFAILURE:		ENGINE_ERROR("CDERR_MEMALLOCFAILURE"); break;
		case CDERR_MEMLOCKFAILURE:		ENGINE_ERROR("CDERR_MEMLOCKFAILURE");  break;
		case CDERR_NOHINSTANCE:			ENGINE_ERROR("CDERR_NOHINSTANCE");     break;
		case CDERR_NOHOOK:				ENGINE_ERROR("CDERR_NOHOOK");          break;
		case CDERR_NOTEMPLATE:			ENGINE_ERROR("CDERR_NOTEMPLATE");      break;
		case CDERR_STRUCTSIZE:			ENGINE_ERROR("CDERR_STRUCTSIZE");      break;
		case FNERR_BUFFERTOOSMALL:		ENGINE_ERROR("FNERR_BUFFERTOOSMALL");  break;
		case FNERR_INVALIDFILENAME:		ENGINE_ERROR("FNERR_INVALIDFILENAME"); break;
		case FNERR_SUBCLASSFAILURE:		ENGINE_ERROR("FNERR_SUBCLASSFAILURE"); break;
		default: ENGINE_INFO("Save as cancelled.");
		}
	}

	return std::wstring(filename);
	//return std::string(ws.begin(), ws.end());
#endif // __WINDOWS__
	return std::wstring();
}