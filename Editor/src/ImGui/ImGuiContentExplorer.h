#pragma once

#include "include.h"

#include <filesystem>

#include "FileSystem/Directory.h"

struct History
{
protected:
	std::vector<std::filesystem::path> paths;
	int currentPathIndex = 0;
public:
	inline bool CanGoBack()
	{
		return currentPathIndex > 0;
	}
	inline bool CanGoForward()
	{
		return currentPathIndex >= 0 && currentPathIndex < (int)paths.size() - 1;
	}

	bool GoBack()
	{
		if (CanGoBack())
		{
			--currentPathIndex;

			return true;
		}
		return false;
	}

	bool GoForward()
	{
		if (CanGoForward())
		{
			++currentPathIndex;
			return true;
		}
		return false;
	}

	bool IsValid() const { return (currentPathIndex >= 0 && currentPathIndex < (int)paths.size() && paths.size() > 0); }
	const std::filesystem::path* GetCurrentFolder() const { return IsValid() ? &paths[currentPathIndex] : new std::filesystem::path(); }

	bool SwitchTo(const std::filesystem::path fi)
	{
		if (fi.string().length() == 0)
			return false;
		if (currentPathIndex >= 0 && paths.size() > 0)
		{
			const std::filesystem::path lastPath = paths[currentPathIndex];
			if (lastPath == fi)
				return false;
		}
		paths.push_back(fi);
		currentPathIndex = (int)paths.size() - 1;
		return true;
	}
};

class ImGuiContentExplorer
	:public Layer
{
public:
	ImGuiContentExplorer(bool* show);
	~ImGuiContentExplorer() = default;

	void OnAttach() override;
	void OnImGuiRender() override;
private:
	std::filesystem::path GetPathForSplitPathIndex(int index);
	void CalculateBrowsingDataTableSizes(const ImVec2& childWindowSize = ImVec2(-1, -1));
private:
	bool* m_Show;

	std::string FileDialog(const wchar_t* title, const wchar_t* filter);

	TextureLibrary2D m_TextureLibrary;

	std::vector<std::filesystem::path> dirs, files;
	std::filesystem::path currentPath;
	std::vector<std::string> currentSplitPath;

	Sorting sortingMode = Sorting::ALPHABETIC;
	History history;

	bool editLocationCheckButtonPressed = false;
	bool forceRescan = true;

	int totalNumBrowsingEntries;
	int numBrowsingColumns;
	int numBrowsingEntriesPerColumn;
};