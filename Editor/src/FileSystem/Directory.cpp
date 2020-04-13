#include "Directory.h"

#include <algorithm>
#include <include.h>

std::vector<std::filesystem::path> Directory::GetDirectories(const std::filesystem::path& path, Sorting sorting)
{
	std::vector<std::filesystem::path> result;

	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
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

std::vector<std::filesystem::path> Directory::GetFiles(const std::filesystem::path& path, Sorting sorting)
{
	std::vector<std::filesystem::path> result;
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
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

std::vector<std::filesystem::path> Directory::GetFiles(const char* directoryName, const char* wantedExtensions, const char* unwantedExtensions, Sorting sorting)
{
	std::vector<std::filesystem::path> result;

	try
	{
		if (wantedExtensions != "")
		{
			std::vector<std::string> extensions = SplitString(wantedExtensions, ';');
			for (const auto& entry : std::filesystem::directory_iterator(directoryName))
			{
				if (entry.is_regular_file() && std::find(extensions.begin(), extensions.end(), entry.path().extension()) != extensions.end())
					result.push_back(entry.path());
			}
		}
		else
		{
			std::vector<std::string> extensions = SplitString(unwantedExtensions, ';');
			for (const auto& entry : std::filesystem::directory_iterator(directoryName))
			{
				if (entry.is_regular_file() && std::find(extensions.begin(), extensions.end(), entry.path().extension()) == extensions.end())
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

void SortingHelper::Sort(std::vector<std::filesystem::path>& paths, Sorting sortOrder)
{
	switch (sortOrder)
	{
	case Sorting::ALPHABETIC:					std::sort(paths.begin(), paths.end(), CompareAlphabetic);			break;
	case Sorting::ALPHBETIC_INVERSE:			std::sort(paths.begin(), paths.end(), CompareAlphabeticInverse);	break;
	case Sorting::LAST_MODIFICATION:			std::sort(paths.begin(), paths.end(), CompareModification);			break;
	case Sorting::LAST_MODIFICATION_REVERSE:	std::sort(paths.begin(), paths.end(), CompareModificationInverse);	break;
	case Sorting::SIZE:							std::sort(paths.begin(), paths.end(), CompareSize);					break;
	case Sorting::SIZE_INVERSE:					std::sort(paths.begin(), paths.end(), CompareSizeInverse);			break;
	case Sorting::TYPE:							std::sort(paths.begin(), paths.end(), CompareType);					break;
	case Sorting::TYPE_INVERSE:					std::sort(paths.begin(), paths.end(), CompareTypeInverse);			break;
	default: break;
	}
}

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