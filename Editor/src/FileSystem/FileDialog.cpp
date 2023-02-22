#include "FileDialog.h"

#include "Engine.h"

#ifdef __WINDOWS__
#include <commdlg.h>
#include <cderr.h>

std::optional<std::wstring> FileDialog::Open(const wchar_t *title, const wchar_t *filter)
{
	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
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
		case CDERR_DIALOGFAILURE:
			ENGINE_ERROR("CDERR_DIALOGFAILURE");
			break;
		case CDERR_FINDRESFAILURE:
			ENGINE_ERROR("CDERR_FINDRESFAILURE");
			break;
		case CDERR_INITIALIZATION:
			ENGINE_ERROR("CDERR_INITIALIZATION");
			break;
		case CDERR_LOADRESFAILURE:
			ENGINE_ERROR("CDERR_LOADRESFAILURE");
			break;
		case CDERR_LOADSTRFAILURE:
			ENGINE_ERROR("CDERR_LOADSTRFAILURE");
			break;
		case CDERR_LOCKRESFAILURE:
			ENGINE_ERROR("CDERR_LOCKRESFAILURE");
			break;
		case CDERR_MEMALLOCFAILURE:
			ENGINE_ERROR("CDERR_MEMALLOCFAILURE");
			break;
		case CDERR_MEMLOCKFAILURE:
			ENGINE_ERROR("CDERR_MEMLOCKFAILURE");
			break;
		case CDERR_NOHINSTANCE:
			ENGINE_ERROR("CDERR_NOHINSTANCE");
			break;
		case CDERR_NOHOOK:
			ENGINE_ERROR("CDERR_NOHOOK");
			break;
		case CDERR_NOTEMPLATE:
			ENGINE_ERROR("CDERR_NOTEMPLATE");
			break;
		case CDERR_STRUCTSIZE:
			ENGINE_ERROR("CDERR_STRUCTSIZE");
			break;
		case FNERR_BUFFERTOOSMALL:
			ENGINE_ERROR("FNERR_BUFFERTOOSMALL");
			break;
		case FNERR_INVALIDFILENAME:
			ENGINE_ERROR("FNERR_INVALIDFILENAME");
			break;
		case FNERR_SUBCLASSFAILURE:
			ENGINE_ERROR("FNERR_SUBCLASSFAILURE");
			break;
		default:
			ENGINE_INFO("File open cancelled.");
		}

		return std::nullopt;
	}

	return std::wstring(filename);
}

std::optional<std::vector<std::wstring>> FileDialog::MultiOpen(const wchar_t *title, const wchar_t *filter)
{
	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if (!GetOpenFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:
			ENGINE_ERROR("CDERR_DIALOGFAILURE");
			break;
		case CDERR_FINDRESFAILURE:
			ENGINE_ERROR("CDERR_FINDRESFAILURE");
			break;
		case CDERR_INITIALIZATION:
			ENGINE_ERROR("CDERR_INITIALIZATION");
			break;
		case CDERR_LOADRESFAILURE:
			ENGINE_ERROR("CDERR_LOADRESFAILURE");
			break;
		case CDERR_LOADSTRFAILURE:
			ENGINE_ERROR("CDERR_LOADSTRFAILURE");
			break;
		case CDERR_LOCKRESFAILURE:
			ENGINE_ERROR("CDERR_LOCKRESFAILURE");
			break;
		case CDERR_MEMALLOCFAILURE:
			ENGINE_ERROR("CDERR_MEMALLOCFAILURE");
			break;
		case CDERR_MEMLOCKFAILURE:
			ENGINE_ERROR("CDERR_MEMLOCKFAILURE");
			break;
		case CDERR_NOHINSTANCE:
			ENGINE_ERROR("CDERR_NOHINSTANCE");
			break;
		case CDERR_NOHOOK:
			ENGINE_ERROR("CDERR_NOHOOK");
			break;
		case CDERR_NOTEMPLATE:
			ENGINE_ERROR("CDERR_NOTEMPLATE");
			break;
		case CDERR_STRUCTSIZE:
			ENGINE_ERROR("CDERR_STRUCTSIZE");
			break;
		case FNERR_BUFFERTOOSMALL:
			ENGINE_ERROR("FNERR_BUFFERTOOSMALL");
			break;
		case FNERR_INVALIDFILENAME:
			ENGINE_ERROR("FNERR_INVALIDFILENAME");
			break;
		case FNERR_SUBCLASSFAILURE:
			ENGINE_ERROR("FNERR_SUBCLASSFAILURE");
			break;
		default:
			ENGINE_INFO("File open cancelled.");
		}

		return std::nullopt;
	}

	std::vector<std::wstring> files;

	wchar_t *str = ofn.lpstrFile;
	std::wstring directory = filename;
	str += (directory.length() + 1);
	if (!*str)
	{
		files.push_back(directory);
		return files;
	}
	while (*str)
	{
		std::wstring file = str;
		str += (file.length() + 1);
		std::wstring ws(file);
		files.push_back(directory + ws);
	}

	return files;
}

std::optional<std::wstring> FileDialog::SaveAs(const wchar_t *title, const wchar_t *filter)
{
	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (!GetSaveFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:
			ENGINE_ERROR("CDERR_DIALOGFAILURE");
			break;
		case CDERR_FINDRESFAILURE:
			ENGINE_ERROR("CDERR_FINDRESFAILURE");
			break;
		case CDERR_INITIALIZATION:
			ENGINE_ERROR("CDERR_INITIALIZATION");
			break;
		case CDERR_LOADRESFAILURE:
			ENGINE_ERROR("CDERR_LOADRESFAILURE");
			break;
		case CDERR_LOADSTRFAILURE:
			ENGINE_ERROR("CDERR_LOADSTRFAILURE");
			break;
		case CDERR_LOCKRESFAILURE:
			ENGINE_ERROR("CDERR_LOCKRESFAILURE");
			break;
		case CDERR_MEMALLOCFAILURE:
			ENGINE_ERROR("CDERR_MEMALLOCFAILURE");
			break;
		case CDERR_MEMLOCKFAILURE:
			ENGINE_ERROR("CDERR_MEMLOCKFAILURE");
			break;
		case CDERR_NOHINSTANCE:
			ENGINE_ERROR("CDERR_NOHINSTANCE");
			break;
		case CDERR_NOHOOK:
			ENGINE_ERROR("CDERR_NOHOOK");
			break;
		case CDERR_NOTEMPLATE:
			ENGINE_ERROR("CDERR_NOTEMPLATE");
			break;
		case CDERR_STRUCTSIZE:
			ENGINE_ERROR("CDERR_STRUCTSIZE");
			break;
		case FNERR_BUFFERTOOSMALL:
			ENGINE_ERROR("FNERR_BUFFERTOOSMALL");
			break;
		case FNERR_INVALIDFILENAME:
			ENGINE_ERROR("FNERR_INVALIDFILENAME");
			break;
		case FNERR_SUBCLASSFAILURE:
			ENGINE_ERROR("FNERR_SUBCLASSFAILURE");
			break;
		default:
			ENGINE_INFO("Save as cancelled.");
		}
		return std::nullopt;
	}

	return std::wstring(filename);
}

#elif defined(__linux__)

#include <linux/limits.h>

std::optional<std::wstring> FileDialog::Open(const wchar_t *title, const wchar_t *filter)
{
	char call[2048];

	std::string outputString;

	//sprintf(call, "zenity --file-selection --modal --title=\"%ls\" --file-filter=\"%ls\" 2>&1", title, filter);
	sprintf(call, "zenity --file-selection --modal --title=\"%ls\"  2>&1", title);

	FILE *f = popen(call, "r");

	if (f)
	{
		char outpLine[PATH_MAX];
		//while (!feof(f))
		//{
			if(fgets(outpLine, PATH_MAX, f) != NULL)
			{
				outputString += outpLine;

				rtrim(outputString);
			}
		//}

		if(!outputString.empty())
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(outputString);
	}
	return std::nullopt;
}

std::optional<std::vector<std::wstring>> FileDialog::MultiOpen(const wchar_t *title, const wchar_t *filter)
{
	char call[2048];
	std::string outputString;

	sprintf(call, "zenity --file-selection --modal --title=\"%ls\" --file-filter=\"%ls\" --multiple 2>&l", title, filter);

	FILE *f = popen(call, "r");

	if(f)
	{
		char outpLine[PATH_MAX];
		std::vector<std::wstring> files;

		while (!feof(f))
		{
			if(fgets(outpLine, PATH_MAX, f)!= NULL)
			{
				outputString += outpLine;

				if(!outputString.empty())
					files.push_back(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(outputString));
			}
		}

		if(!files.empty())
			return files;
	}
	return std::nullopt;
}

std::optional<std::wstring> FileDialog::SaveAs(const wchar_t *title, const wchar_t *filter)
{
	char call[2048];
	std::string outputString;

	//sprintf(call, "zenity --file-selection --modal --title=\"%ls\" --file-filter=\"%ls\" --save", title, filter);
	sprintf(call, "zenity --file-selection --modal --title=\"%ls\" --save", title);

	FILE *f = popen(call, "r");

	if (f)
	{
		char outpLine[PATH_MAX];
		while (!feof(f))
		{
			if(fgets(outpLine, PATH_MAX, f) != NULL)
			{
				outputString += outpLine;

				rtrim(outputString);
			}
		}

		if(!outputString.empty())
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(outputString);
	}

	return std::nullopt;
}

#else

std::optional<std::wstring> FileDialog::Open(const wchar_t *title, const wchar_t *filter)
{
	return std::nullopt;
}
std::optional<std::vector<std::wstring>> FileDialog::MultiOpen(const wchar_t *title, const wchar_t *filter)
{
	return std::nullopt;
}

std::optional<std::wstring> FileDialog::SaveAs(const wchar_t *title, const wchar_t *filter)
{
	return std::nullopt;
}

#endif // __WINDOWS__