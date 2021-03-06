#pragma once

#include <filesystem>

#include "Engine.h"

#include "FileSystem/Directory.h"
#include "Interfaces/ICopyable.h"

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

class ContentExplorerPanel
	:public Layer, public ICopyable
{
public:
	explicit ContentExplorerPanel(bool* show);
	~ContentExplorerPanel() = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;

	virtual void Copy() override;
	virtual void Cut() override;
	virtual void Paste() override;
	virtual void Duplicate() override;
	virtual void Delete() override;
	virtual void SelectAll() override;
	virtual bool HasSelection() const override;

	virtual bool IsReadOnly() const override { return false; }

	void SwitchTo(const std::filesystem::path& path);

	void CreateNewScene();

private:
	std::filesystem::path GetPathForSplitPathIndex(int index);
	void CalculateBrowsingDataTableSizes(const ImVec2& childWindowSize = ImVec2(-1, -1));
	void HandleKeyboardInputs();
	void RightClickMenu();
	bool Rename();

	void OpenAllSelectedItems();
private:
	bool* m_Show;

	TextureLibrary2D m_TextureLibrary;

	std::vector<std::filesystem::path> m_Dirs, m_Files;
	std::filesystem::path m_CurrentPath;
	std::vector<std::string> m_CurrentSplitPath;

	std::vector<bool> m_SelectedDirs;
	std::vector<bool> m_SelectedFiles;

	int m_NumberSelected;
	ImVec2 m_CurrentSelectedPosition;
	std::filesystem::path m_CurrentSelectedPath;

	Sorting m_SortingMode = Sorting::ALPHABETIC;
	History m_History;

	bool m_EditLocationCheckButtonPressed = false;
	bool m_ForceRescan = true;

	int m_TotalNumBrowsingEntries;
	int m_NumBrowsingColumns;
	int m_NumBrowsingEntriesPerColumn;

	std::vector<std::filesystem::path> m_CopiedPaths;
	bool m_Cut;
};