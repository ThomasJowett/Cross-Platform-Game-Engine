#pragma once

#include <filesystem>

#include "Utilities/StringUtils.h"

#include "FileSystem/Directory.h"
#include "Utilities/FileWatcher.h"
#include "Interfaces/ICopyable.h"
#include "Asset/Texture.h"
#include "Viewers/ViewerManager.h"

#include "imgui/imgui.h"

struct History
{
protected:
	std::vector<std::filesystem::path> paths;
	int currentPathIndex = 0;
	std::filesystem::path m_RootPath;
public:
	History() = default;

	void SetRootPath(const std::filesystem::path& rootPath)
	{
		m_RootPath = rootPath;
	}

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
		if (paths.size() > 0)
		{
			std::filesystem::path current = paths[currentPathIndex];
			if (current == m_RootPath || current.string().length() <= m_RootPath.string().length())
				return false;
			return SplitString(current.string(), std::filesystem::path::preferred_separator).size() > 1;
		}
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
			std::string newPath = path.substr(0, path.find_last_of(std::filesystem::path::preferred_separator));

			SwitchTo(newPath);
			return true;
		}
		return false;
	}

	bool IsValid() const { return (currentPathIndex >= 0 && currentPathIndex < (int)paths.size() && paths.size() > 0); }
	const std::filesystem::path* GetCurrentFolder() const { return IsValid() ? &paths[currentPathIndex] : new std::filesystem::path(); }

	bool SwitchTo(const std::filesystem::path fi)
	{
		if (fi.string().empty())
			return false;

		std::filesystem::path normalizedFi = std::filesystem::absolute(fi);
		
		// Ensure the new path is within the root path
		std::string fiStr = normalizedFi.string();
		std::string rootStr = m_RootPath.string();
		if (fiStr.find(rootStr) != 0)
		{
			return false;
		}

		if (currentPathIndex >= 0 && !paths.empty())
		{
			const std::filesystem::path& lastPath = paths[currentPathIndex];
			if (lastPath == normalizedFi)
				return false;
		}

		if (currentPathIndex < (int)paths.size() - 1)
		{
			paths.erase(paths.begin() + currentPathIndex + 1, paths.end());
		}

		paths.push_back(normalizedFi);
		currentPathIndex = (int)paths.size() - 1;
		return true;
	}

	void Clear()
	{
		paths.clear();
		currentPathIndex = 0;
	}
};

class ContentExplorerPanel
	:public Layer, public ICopyable
{
	enum class ZoomLevel
	{
		List = 0,
		Thumbnails = 1,
		Details = 2
	};

public:
	explicit ContentExplorerPanel(bool* show);
	~ContentExplorerPanel();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

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
	void CreateNewMaterial();
	void CreateNewLuaScript();
	void CreateNewTileset(const std::filesystem::path* path = nullptr);
	void CreateNewSpriteSheet(const std::filesystem::path* path = nullptr);
	void CreateNewPhysicsMaterial();
	void CreateNewBehaviourTree();

private:
	std::filesystem::path GetPathForSplitPathIndex(int index);
	void CalculateBrowsingDataTableSizes(const ImVec2& childWindowSize = ImVec2(-1, -1));
	void HandleKeyboardInputs();
	void RightClickMenu();
	void CreateNewMenu();
	bool Rename();
	void UpdateReferencesAfterRename(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

	void OpenAllSelectedItems();
	void OpenItem(size_t index);
	void PerformDelete(const std::vector<std::filesystem::path>& targets, const std::vector<std::filesystem::path>& affectedReferences);

	void ItemContextMenu(size_t index, bool isDirectory, const std::string& itemName);
	void CreateDragDropSource(size_t index);
	void ClearSelected();

	std::string GetFileIconForFileType(FileType type) const;
	std::string GetFileIconForFileType(const std::filesystem::path& assetPath) const;

private:
	bool* m_Show;

	TextureLibrary2D m_TextureLibrary;

	std::vector<std::filesystem::path> m_Dirs, m_Files;
	std::filesystem::path m_CurrentPath;
	std::vector<std::string> m_CurrentSplitPath;

	std::vector<bool> m_SelectedDirs;
	std::vector<bool> m_SelectedFiles;

	int m_LastSelectedDir = -1;
	int m_LastSelectedFile = -1;

	int m_NumberSelected;
	std::filesystem::path m_CurrentSelectedPath;

	Sorting m_SortingMode = Sorting::ALPHABETIC;
	ZoomLevel m_ZoomLevel = ZoomLevel::List;
	History m_History;

	bool m_EditLocationCheckButtonPressed = false;
	bool m_ForceRescan = true;

	int m_TotalNumBrowsingEntries;
	int m_NumBrowsingColumns;
	int m_NumBrowsingEntriesPerColumn;

	std::vector<std::filesystem::path> m_CopiedPaths;
	bool m_Cut;

	ImGuiTextFilter* m_TextFilter;
	FileType m_TypeFilter = FileType::UNKNOWN;

	FileWatcher m_FileWatcher;

	char m_CurrentPathInputBuffer[1024] = "";
	char m_RenameInputBuffer[1024] = "";

	bool m_TryingToChangeScene = false;
	bool m_Renaming = false;

	bool m_ShowDeleteConfirmation = false;
	std::vector<std::filesystem::path> m_PendingDeletePaths;
	std::vector<std::filesystem::path> m_PendingDeleteReferences;
};