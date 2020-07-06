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
	inline bool CanGoUp()
	{
		if(paths.size() > 0)
			return SplitString(paths[currentPathIndex].string(), '\\').size() > 1;
		return false;
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

	bool GoUp()
	{
		if (CanGoUp())
		{
			std::string path = paths[currentPathIndex].string();
			std::string newPath  = path.substr(0, path.find_last_of('\\'));

			SwitchTo(newPath);
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
	explicit ImGuiContentExplorer(bool* show);
	~ImGuiContentExplorer() = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;

	void Copy();
	void Cut();
	void Paste();
	void Duplicate();
	void Delete();
private:
	std::filesystem::path GetPathForSplitPathIndex(int index);
	void CalculateBrowsingDataTableSizes(const ImVec2& childWindowSize = ImVec2(-1, -1));
	void HandleKeyboardInputs();
	void HandleMouseInputs();
	void RightClickMenu();

	void OpenAllSelectedItems();
private:
	bool* m_Show;

	TextureLibrary2D m_TextureLibrary;

	std::vector<std::filesystem::path> m_Dirs, m_Files;
	std::filesystem::path m_CurrentPath;
	std::vector<std::string> m_CurrentSplitPath;

	std::vector<bool> m_SelectedDirs;
	std::vector<bool> m_SelectedFiles;

	Sorting m_SortingMode = Sorting::ALPHABETIC;
	History m_History;

	bool m_EditLocationCheckButtonPressed = false;
	bool m_ForceRescan = true;

	int m_TotalNumBrowsingEntries;
	int m_NumBrowsingColumns;
	int m_NumBrowsingEntriesPerColumn;
};