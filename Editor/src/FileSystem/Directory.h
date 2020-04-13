#pragma once

#include <vector>
#include <filesystem>

enum class Sorting
{
	ALPHABETIC = 0,
	ALPHBETIC_INVERSE = 1,
	LAST_MODIFICATION = 2,
	LAST_MODIFICATION_REVERSE = 3,
	SIZE = 4,
	SIZE_INVERSE = 5,
	TYPE = 6,
	TYPE_INVERSE = 7
};

class SortingHelper
{
public:
	static void Sort(std::vector<std::filesystem::path>& paths, Sorting sortOrder);
private:
	static bool CompareAlphabetic(std::filesystem::path& first, std::filesystem::path& second);
	static bool CompareAlphabeticInverse(std::filesystem::path& first, std::filesystem::path& second);
	static bool CompareModification(std::filesystem::path& first, std::filesystem::path& second);
	static bool CompareModificationInverse(std::filesystem::path& first, std::filesystem::path& second);
	static bool CompareSize(std::filesystem::path& first, std::filesystem::path& second);
	static bool CompareSizeInverse(std::filesystem::path& first, std::filesystem::path& second);
	static bool CompareType(std::filesystem::path& first, std::filesystem::path& second);
	static bool CompareTypeInverse(std::filesystem::path& first, std::filesystem::path& second);
};

class Directory
{
public:
	static std::vector<std::filesystem::path> GetDirectories(const char* directoryName, Sorting sorting = Sorting::ALPHABETIC);
	static std::vector<std::filesystem::path> GetFiles(const char* directoryName, Sorting sorting = Sorting::ALPHABETIC);

	// e.g. ".txt;.jpg;.png". To use unwantedExtensions, set wantedExtensions="".
	static std::vector<std::filesystem::path> GetFiles(const char* directoryName, const char* wantedExtensions, const char* unwantedExtensions = NULL, Sorting sorting = Sorting::ALPHABETIC);

	static void Create(const char* directoryName);
};

