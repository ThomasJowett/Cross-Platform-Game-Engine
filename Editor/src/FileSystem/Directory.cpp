#include "Directory.h"
#include "Engine.h"

#include <algorithm>

std::vector<std::filesystem::path> Directory::GetDirectories(const std::filesystem::path& path, Sorting sorting)
{
	std::vector<std::filesystem::path> result;

	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
		{
			if (entry.is_directory())
				result.push_back(entry.path());
		}

		SortingHelper::Sort(result, sorting);
	}
	catch (const std::exception& e)
	{
		CLIENT_ERROR(e.what());
	}

	return result;
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::vector<std::filesystem::path> Directory::GetFiles(const std::filesystem::path& path, Sorting sorting)
{
	std::vector<std::filesystem::path> result;
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
		{
			if (entry.is_regular_file())
				result.push_back(entry.path());
		}

		SortingHelper::Sort(result, sorting);
	}
	catch (const std::exception& e)
	{
		CLIENT_ERROR(e.what());
	}
	return result;
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::vector<std::filesystem::path> Directory::GetFilesRecursive(const std::filesystem::path& path, std::vector<std::string> wantedExtensions, std::vector<std::string> unwantedExtensions, Sorting sorting)
{
	std::vector<std::filesystem::path> result;

	try
	{
		if (!wantedExtensions.empty())
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
			{
				if (entry.is_regular_file() && std::find(wantedExtensions.begin(), wantedExtensions.end(), entry.path().extension()) != wantedExtensions.end())
					result.push_back(entry.path());
			}
		}
		else
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
			{
				if (entry.is_regular_file() && std::find(unwantedExtensions.begin(), unwantedExtensions.end(), entry.path().extension()) == unwantedExtensions.end())
					result.push_back(entry.path());
			}
		}

		SortingHelper::Sort(result, sorting);
	}
	catch (const std::exception& e)
	{
		CLIENT_ERROR(e.what());
	}
	return result;
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::filesystem::path Directory::GetNextNewFileName(const std::filesystem::path& directory, const std::string& filename, const std::string& extension)
{
	std::string newFilepath = (directory / filename).string();

	int suffix = 1;
	if(std::filesystem::exists(newFilepath + extension))
	{
		while (std::filesystem::exists(newFilepath + " (" + std::to_string(suffix)+ ")" + extension))
		{
			suffix++;
		}
		newFilepath += '(' + std::to_string(suffix) + ')';
	}

	newFilepath += extension;

	return std::filesystem::path(newFilepath);
}

void SortingHelper::Sort(std::vector<std::filesystem::path>& paths, Sorting sortOrder)
{
	switch (sortOrder)
	{
	case Sorting::ALPHABETIC:					std::sort(paths.begin(), paths.end(), CompareAlphabetic);			break;
	case Sorting::ALPHABETIC_INVERSE:			std::sort(paths.begin(), paths.end(), CompareAlphabeticInverse);	break;
	case Sorting::LAST_MODIFICATION:			std::sort(paths.begin(), paths.end(), CompareModification);			break;
	case Sorting::LAST_MODIFICATION_INVERSE:	std::sort(paths.begin(), paths.end(), CompareModificationInverse);	break;
	case Sorting::SIZE:							std::sort(paths.begin(), paths.end(), CompareSize);					break;
	case Sorting::SIZE_INVERSE:					std::sort(paths.begin(), paths.end(), CompareSizeInverse);			break;
	case Sorting::TYPE:							std::sort(paths.begin(), paths.end(), CompareType);					break;
	case Sorting::TYPE_INVERSE:					std::sort(paths.begin(), paths.end(), CompareTypeInverse);			break;
	default: break;
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SortingHelper::CompareAlphabetic(std::filesystem::path& first, std::filesystem::path& second)
{
	return first.filename().string() < second.filename().string();
}

bool SortingHelper::CompareAlphabeticInverse(std::filesystem::path& first, std::filesystem::path& second)
{
	return first.filename().string() > second.filename().string();
}

bool SortingHelper::CompareModification(std::filesystem::path& first, std::filesystem::path& second)
{
	return std::filesystem::last_write_time(first) > std::filesystem::last_write_time(second);
}

bool SortingHelper::CompareModificationInverse(std::filesystem::path& first, std::filesystem::path& second)
{
	return std::filesystem::last_write_time(first) < std::filesystem::last_write_time(second);
}

bool SortingHelper::CompareSize(std::filesystem::path& first, std::filesystem::path& second)
{
	return std::filesystem::file_size(first) < std::filesystem::file_size(second);
}

bool SortingHelper::CompareSizeInverse(std::filesystem::path& first, std::filesystem::path& second)
{
	return std::filesystem::file_size(first) > std::filesystem::file_size(second);
}

bool SortingHelper::CompareType(std::filesystem::path& first, std::filesystem::path& second)
{
	return first.extension().string() < second.extension().string();
}

bool SortingHelper::CompareTypeInverse(std::filesystem::path& first, std::filesystem::path& second)
{
	return first.extension().string() > second.extension().string();
}