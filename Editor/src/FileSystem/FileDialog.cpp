#include "FileDialog.h"

#ifdef __WINDOWS__
#include <commdlg.h>
#include <cderr.h>

std::optional<std::wstring> FileDialog::Open(const wchar_t* title, const wchar_t* filter)
{
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
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

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

        return std::nullopt;
	}

	return std::wstring(filename);
}

std::optional<std::vector<std::wstring>> FileDialog::MultiOpen(const wchar_t* title, const wchar_t* filter)
{
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
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

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

        return std::nullopt;
	}

	std::vector<std::wstring> files;

	wchar_t* str = ofn.lpstrFile;
	std::wstring directory = filename;
	str += (directory.length() + 1);
	if (!*str)
	{
		files.push_back(directory);
		return files;
	}
	while (*str) {
		std::wstring file = str;
		str += (file.length() + 1);
		std::wstring ws(file);
		files.push_back(directory + ws);
	}

	return files;
}

std::optional<std::wstring> FileDialog::SaveAs(const wchar_t* title, const wchar_t* filter)
{
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
        return std::nullopt;
	}

	return std::wstring(filename);
}

#else

std::optional<std::wstring> FileDialog::Open(const wchar_t* title, const wchar_t* filter)
{
    return std::nullopt;
}
std::optional<std::vector<std::wstring>> FileDialog::MultiOpen(const wchar_t* title, const wchar_t* filter)
{
    return std::nullopt;
}

std::optional<std::wstring> FileDialog::SaveAs(const wchar_t* title, const wchar_t* filter)
{
    return std::nullopt;
}

#endif // __WINDOWS__