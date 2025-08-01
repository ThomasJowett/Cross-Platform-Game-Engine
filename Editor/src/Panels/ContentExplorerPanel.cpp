#include "ContentExplorerPanel.h"

#include <chrono>
#include <iomanip>
#include <fstream>

#include "imgui/imgui.h"

#include "Engine.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome6.h"

#include "FileSystem/FileDialog.h"

#include "Importers/ImportManager.h"

#include "Scene/SceneManager.h"
#include "History/HistoryManager.h"

#include "Fonts/Fonts.h"

static std::filesystem::path s_IconDirectory;

template <typename TP>
std::time_t to_time_t(TP tp)
{
	using namespace std::chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
		+ system_clock::now());
	return system_clock::to_time_t(sctp);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::Paste()
{
	if (m_Cut)
	{
		// if the files have been cut then rename them
		for (auto path : m_CopiedPaths)
		{
			try
			{
				std::filesystem::rename(path, m_CurrentPath / path.filename());
			}
			catch (const std::filesystem::filesystem_error&)
			{
				try
				{
					std::filesystem::copy_file(path, m_CurrentPath / path.filename(), std::filesystem::copy_options::skip_existing);
				}
				catch (const std::filesystem::filesystem_error& e)
				{
					CLIENT_ERROR("Could not paste cut files: {0}", e.what());
				}
			}
		}
		m_Cut = false;
	}
	else
	{
		for (auto path : m_CopiedPaths)
		{
			auto target = m_CurrentPath / path.filename();

			if (std::filesystem::exists(target))
			{
				int suffix = 1;
				std::string extension = target.extension().string();

				std::filesystem::path targetNoExt = target;
				targetNoExt.replace_extension("");

				std::filesystem::path temp = targetNoExt.string() + "- Copy" + extension;

				while (std::filesystem::exists(temp))
				{
					suffix++;
					temp = targetNoExt.string() + "- Copy (" + std::to_string(suffix) + ')' + extension;
				}

				target = temp;
			}

			try
			{
				std::filesystem::copy_file(path, target, std::filesystem::copy_options::skip_existing);
			}
			catch (std::exception& e)
			{
				CLIENT_ERROR(e.what());
			}
		}
	}

	m_ForceRescan = true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::Duplicate()
{
	Copy();
	Paste();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::Delete()
{
	if (m_NumberSelected == 1)
		std::filesystem::remove_all(m_CurrentSelectedPath);
	else
	{
		for (size_t i = 0; i < m_SelectedFiles.size(); i++)
		{
			if (m_SelectedFiles[i])
				std::filesystem::remove(m_Files[i]);
		}

		for (size_t i = 0; i < m_SelectedDirs.size(); i++)
		{
			if (m_SelectedDirs[i])
				std::filesystem::remove_all(m_Dirs[i]);
		}
	}

	m_ForceRescan = true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::SelectAll()
{
	for (auto file : m_SelectedFiles)
		file = true;

	for (auto dir : m_SelectedDirs)
		dir = true;

	m_NumberSelected = (uint32_t)(m_SelectedDirs.size() + m_SelectedFiles.size());
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool ContentExplorerPanel::HasSelection() const
{
	return m_NumberSelected > 0;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool ContentExplorerPanel::Rename()
{
	memset(m_RenameInputBuffer, 0, sizeof(m_RenameInputBuffer));
	for (int i = 0; i < m_CurrentSelectedPath.filename().string().length(); i++)
	{
		m_RenameInputBuffer[i] = m_CurrentSelectedPath.filename().string()[i];
	}
	static bool hasFocus = false;
	static bool once = false;

	if (!hasFocus)
	{
		ImGui::SetKeyboardFocusHere();
		hasFocus = true;
	}

	if (ImGui::InputText("##RenameBox", m_RenameInputBuffer, sizeof(m_RenameInputBuffer),
		ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (!std::filesystem::exists(m_CurrentPath / m_RenameInputBuffer))
		{
			std::filesystem::rename(m_CurrentSelectedPath, m_CurrentPath / m_RenameInputBuffer);
			m_ForceRescan = true;
		}
		else
		{
			CLIENT_ERROR("Could not name folder: Folder already exists!");
		}

		ImGui::CloseCurrentPopup();
		m_Renaming = false;
		hasFocus = false;
		once = false;
		return true;
	}

	if (!ImGui::IsItemActive())
	{
		if (once)
		{
			ImGui::CloseCurrentPopup();
			m_Renaming = false;
			hasFocus = false;
			once = false;
			return false;
		}
		once = true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::SwitchTo(const std::filesystem::path& path)
{
	m_CurrentPath = path;

	if (std::filesystem::is_regular_file(m_CurrentPath))
	{
		m_CurrentPath.remove_filename();
		std::string str = m_CurrentPath.string();
		str.pop_back();
		m_CurrentPath = str;
	}

	m_CurrentSplitPath = SplitString(m_CurrentPath.string(), std::filesystem::path::preferred_separator);
	m_History.SwitchTo(m_CurrentPath);
	m_FileWatcher.SetPathToWatch(m_CurrentPath);
	m_ForceRescan = true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::CreateNewScene()
{
	std::filesystem::path newSceneFilepath = Directory::GetNextNewFileName(m_CurrentPath, "New Scene", ".scene");

	SceneManager::CreateScene(newSceneFilepath);

	m_ForceRescan = true;

	m_CurrentSelectedPath = newSceneFilepath;
}

void ContentExplorerPanel::CreateNewMaterial()
{
	std::filesystem::path newMaterialFilepath = Directory::GetNextNewFileName(m_CurrentPath, "New Material", ".material");

	Material material("Standard", Colours::WHITE);
	material.SaveMaterial(newMaterialFilepath);
	m_ForceRescan = true;
	m_CurrentSelectedPath = newMaterialFilepath;
}

void ContentExplorerPanel::CreateNewLuaScript()
{
	std::filesystem::path newLuaScriptFilepath = Directory::GetNextNewFileName(m_CurrentPath, "New Script", ".lua");

	std::ofstream file(newLuaScriptFilepath);

	if (file.is_open())
	{
		file << "-- Called when entity is created" << std::endl;
		file << "function OnCreate()" << std::endl;
		file << std::endl;
		file << "end" << std::endl;
		file << std::endl;
		file << "-- Called once per frame" << std::endl;
		file << "function OnUpdate(deltaTime)" << std::endl;
		file << std::endl;
		file << "end" << std::endl;
		file << "-- Called on a fixed interval" << std::endl;
		file << "function OnFixedUpdate()" << std::endl;
		file << std::endl;
		file << "end" << std::endl;
		file << "-- Called when entity is destroyed" << std::endl;
		file << "function OnDestroy()" << std::endl;
		file << std::endl;
		file << "end" << std::endl;
	}
	file.close();

	m_ForceRescan = true;
	m_CurrentSelectedPath = newLuaScriptFilepath;
}

void ContentExplorerPanel::CreateNewTileset(const std::filesystem::path* path)
{
	std::filesystem::path newTilesetPath = Directory::GetNextNewFileName(m_CurrentPath, "New Tileset", ".tileset");

	Tileset tileset;
	if (path)
	{
		tileset.SetSubTexture(CreateRef<SubTexture2D>(AssetManager::GetTexture(*path), 32, 32));
	}

	tileset.SaveAs(newTilesetPath);
	m_ForceRescan = true;
	m_CurrentSelectedPath = newTilesetPath;
}

void ContentExplorerPanel::CreateNewSpriteSheet(const std::filesystem::path* path)
{
	std::string filename = "New Sprite Sheet";
	if (path)
		filename = path->filename().string();
	std::filesystem::path newSpriteSheetPath = Directory::GetNextNewFileName(m_CurrentPath, filename, ".spritesheet");

	SpriteSheet spritesheet;

	if (path)
	{
		spritesheet.SetSubTexture(CreateRef<SubTexture2D>(AssetManager::GetTexture(*path), 32, 32));
	}
	spritesheet.SaveAs(newSpriteSheetPath);
	m_ForceRescan = true;
	m_CurrentSelectedPath = newSpriteSheetPath;
}

void ContentExplorerPanel::CreateNewPhysicsMaterial()
{
	std::filesystem::path newPhysicsMaterial = Directory::GetNextNewFileName(m_CurrentPath, "New Physics Material", ".physicsmaterial");

	PhysicsMaterial physMat;

	physMat.SaveAs(newPhysicsMaterial);
	m_ForceRescan = true;
	m_CurrentSelectedPath = newPhysicsMaterial;
}

void ContentExplorerPanel::CreateNewBehaviourTree()
{
	std::filesystem::path newBehaviourTree = Directory::GetNextNewFileName(m_CurrentPath, "New Behaviour Tree", ".behaviourtree");

	BehaviourTree::BehaviourTree behaviourTree;

	BehaviourTree::Serializer::Serialize(newBehaviourTree, &behaviourTree);

	m_ForceRescan = true;
	m_CurrentSelectedPath = newBehaviourTree;
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::filesystem::path ContentExplorerPanel::GetPathForSplitPathIndex(int index)
{
	std::string path;
	for (int i = 0; i <= index; i++)
	{
		path += m_CurrentSplitPath[i];
		if (i != index)
			path += std::filesystem::path::preferred_separator;
	}

	return std::filesystem::path(path);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::CalculateBrowsingDataTableSizes(const ImVec2& childWindowSize)
{
	int approxNumEntriesPerColumn = 20;
	if (childWindowSize.y > 0)
	{
		int numLinesThatFit = (int)(childWindowSize.y / ImGui::GetTextLineHeightWithSpacing());
		if (numLinesThatFit <= 0)
			numLinesThatFit = 1;
		approxNumEntriesPerColumn = numLinesThatFit;
	}
	m_NumBrowsingColumns = (int)std::ceil((float)(m_TotalNumBrowsingEntries / approxNumEntriesPerColumn));

	if (m_NumBrowsingColumns <= 0)
	{
		m_NumBrowsingColumns = 1;
		m_NumBrowsingEntriesPerColumn = approxNumEntriesPerColumn;
		return;
	}

	if (m_TotalNumBrowsingEntries % approxNumEntriesPerColumn > (approxNumEntriesPerColumn / 2))
		++m_NumBrowsingColumns;

	int maxNumBrowsingColumns = (childWindowSize.x > 0) ? (int)(childWindowSize.x / 100) : 6;

	if (maxNumBrowsingColumns < 1)
		maxNumBrowsingColumns = 1;

	if (m_NumBrowsingColumns > maxNumBrowsingColumns)
		m_NumBrowsingColumns = maxNumBrowsingColumns;

	m_NumBrowsingEntriesPerColumn = m_TotalNumBrowsingEntries / m_NumBrowsingColumns;

	if (m_TotalNumBrowsingEntries % m_NumBrowsingColumns != 0)
		++m_NumBrowsingEntriesPerColumn;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::HandleKeyboardInputs()
{
	ImGuiIO& io = ImGui::GetIO();
	bool shift = io.KeyShift;
	bool ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	bool alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ImGui::IsWindowFocused())
	{
		if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			Delete();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
			Copy();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
			Paste();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
			Cut();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
			Duplicate();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
			SelectAll();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_R)) && m_NumberSelected == 1)
			m_Renaming = true;
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::RightClickMenu()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
	if (ImGui::BeginMenu(ICON_FA_FOLDER_PLUS" New"))
	{
		if (ImGui::Selectable(ICON_FA_FOLDER"\tFolder"))
		{
			std::string newFolderName = (m_CurrentPath / "New folder").string();
			int suffix = 1;

			if (std::filesystem::exists(newFolderName))
			{
				while (std::filesystem::exists(newFolderName + " (" + std::to_string(suffix) + ')'))
				{
					suffix++;
				}

				newFolderName += " (" + std::to_string(suffix) + ')';
			}

			std::filesystem::create_directory(newFolderName);

			m_ForceRescan = true;
			m_Renaming = true;

			m_CurrentSelectedPath = newFolderName;
		}
		if (ImGui::Selectable((GetFileIconForFileType(FileType::SCENE) + "\tScene").c_str()))
		{
			CreateNewScene();
			m_Renaming = true;
		}
		if (ImGui::Selectable((GetFileIconForFileType(FileType::MATERIAL) + "\tMaterial").c_str()))
		{
			CreateNewMaterial();
			m_Renaming = true;
		}
		if (ImGui::Selectable((GetFileIconForFileType(FileType::SCRIPT) + "\tLua Script").c_str()))
		{
			CreateNewLuaScript();
			m_Renaming = true;
		}

		if (ImGui::Selectable((GetFileIconForFileType(FileType::TILESET) + "\tTileset").c_str()))
		{
			CreateNewTileset();
			m_Renaming = true;
		}

		if (ImGui::Selectable((GetFileIconForFileType(FileType::SPRITESHEET) + "\tSprite Sheet").c_str()))
		{
			CreateNewSpriteSheet();
			m_Renaming = true;
		}

		if (ImGui::Selectable((GetFileIconForFileType(FileType::PHYSICSMATERIAL) + "\tPhysics Material").c_str()))
		{
			CreateNewPhysicsMaterial();
			m_Renaming = true;
		}

		if (ImGui::Selectable((GetFileIconForFileType(FileType::BEHAVIOURTREE) + "\tBehaviour Tree").c_str()))
		{
			CreateNewBehaviourTree();
			m_Renaming = true;
		}
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem(ICON_FA_FILE_IMPORT" Import Assets"))
	{
		std::optional<std::vector<std::wstring>> assetPaths = FileDialog::MultiOpen(L"Select Files...",
			L"Any File\0*.*\0"
			L"Film Box (.fbx)\0*.fbx\0"
			L"Wavefront OBJ (.obj)\0*.obj\0"
			L"Tiled Tilemap(.tmx)\0*.tmx\0"
			L"Tiled Tileset(.tsx)\0*.tsx");

		if (assetPaths)
		{
			ImportManager::ImportMultiAssets(assetPaths.value(), m_CurrentPath);
			m_ForceRescan = true;
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl + X", nullptr, m_NumberSelected > 0))
		Cut();
	if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl + C", nullptr, m_NumberSelected > 0))
		Copy();
	if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl + V", nullptr, !m_CopiedPaths.empty()))
		Paste();
	if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl + D", nullptr, m_NumberSelected > 0))
		Duplicate();
	if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, m_NumberSelected > 0))
		Delete();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::OpenAllSelectedItems()
{
	for (size_t i = 0; i < m_Files.size(); i++)
	{
		if (m_SelectedFiles[i])
			OpenItem(i);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::OpenItem(size_t index)
{
	std::filesystem::path relativePath = FileUtils::RelativePath(m_Files[index], Application::GetOpenDocumentDirectory());
	if (m_Files[index].extension() == ".scene") {
		SceneManager::ChangeSceneState(SceneState::Edit);
		if (SceneManager::IsSceneLoaded() && SceneManager::CurrentScene()->IsDirty()) {
			m_TryingToChangeScene = true;
		}
		else
			SceneManager::ChangeScene(relativePath);
	}
	else
		ViewerManager::OpenViewer(relativePath);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::ItemContextMenu(size_t index, bool isDirectory, const std::string& itemName)
{
	if (ImGui::BeginPopupContextItem(itemName.c_str()))
	{
		// if the file that this context menu is for is not selected then select it and unselect all that is selected

		if (isDirectory)
		{
			if (!m_SelectedDirs[index])
			{
				ClearSelected();

				m_SelectedDirs[index] = true;

				m_NumberSelected = 1;
			}

			m_CurrentSelectedPath = m_Dirs[index];
		}
		else
		{
			if (!m_SelectedFiles[index])
			{
				ClearSelected();

				m_SelectedFiles[index] = true;

				m_NumberSelected = 1;
			}
			m_CurrentSelectedPath = m_Files[index];

			for (const auto& extension : ViewerManager::GetExtensions(FileType::IMAGE))
			{
				if (m_CurrentSelectedPath.extension() == extension)
				{
					if (ImGui::Selectable("Create Tileset"))
					{
						CreateNewTileset(&m_CurrentSelectedPath);
					}
					if (ImGui::Selectable("Create SpriteSheet"))
					{
						CreateNewSpriteSheet(&m_CurrentSelectedPath);
					}
				}
			}
		}

		if (ImGui::Selectable("Rename") && m_NumberSelected == 1)
		{
			m_Renaming = true;
		}

		ImGui::Separator();

		if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl + X", nullptr, m_NumberSelected > 0))
			Cut();
		if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl + C", nullptr, m_NumberSelected > 0))
			Copy();
		if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl + V", nullptr, !m_CopiedPaths.empty()))
			Paste();
		if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl + D", nullptr, m_NumberSelected > 0))
			Duplicate();
		if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, m_NumberSelected > 0))
			Delete();

		ImGui::EndPopup();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::ClearSelected()
{
	m_SelectedDirs.clear();
	m_SelectedDirs.resize(m_Dirs.size());
	m_SelectedFiles.clear();
	m_SelectedFiles.resize(m_Files.size());
	m_LastSelectedFile = -1;
	m_LastSelectedDir = -1;
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::string ContentExplorerPanel::GetFileIconForFileType(FileType type) const
{
	switch (type)
	{
	case FileType::TEXT:
		return ICON_FA_FILE_LINES;
	case FileType::IMAGE:
		return ICON_FA_FILE_IMAGE;
	case FileType::MESH:
		return ICON_FA_SHAPES;
	case FileType::SCENE:
		return ICON_FA_IMAGE;
	case FileType::SCRIPT:
		return ICON_FA_FILE_CODE;
	case FileType::BEHAVIOURTREE:
	case FileType::STATEMACHINE:
	case FileType::VISUALSCRIPT:
		return ICON_FA_DIAGRAM_PROJECT;
	case FileType::AUDIO:
		return ICON_FA_FILE_AUDIO;
	case FileType::MATERIAL:
		return ICON_FA_BOWLING_BALL;
	case FileType::TILESET:
		return ICON_FA_GRIP;
	case FileType::SPRITESHEET:
		return ICON_FA_PHOTO_FILM;
	case FileType::PHYSICSMATERIAL:
		return ICON_FA_VOLLEYBALL;
	case FileType::FONT:
		return ICON_FA_FONT;
	case FileType::UNKNOWN:
		break;
	}
	return ICON_FA_FILE;
}

std::string ContentExplorerPanel::GetFileIconForFileType(const std::filesystem::path& assetPath) const
{
	return GetFileIconForFileType(ViewerManager::GetFileType(assetPath));
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::CreateDragDropSource(size_t index)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("Asset", &m_Files[index], sizeof(std::filesystem::path));
		ImGui::Text("%s", m_Files[index].filename().string().c_str());
		ImGui::EndDragDropSource();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

ContentExplorerPanel::ContentExplorerPanel(bool* show)
	: Layer("ContentExplorer"), m_Show(show),
	m_FileWatcher(std::chrono::seconds(1))
{
	s_IconDirectory = Application::GetWorkingDirectory() / "data" / "Icons";
	m_TotalNumBrowsingEntries = 0;
	m_NumBrowsingColumns = 0;
	m_NumBrowsingEntriesPerColumn = 0;
	m_NumberSelected = 0;
	m_Cut = false;
	m_TextFilter = new ImGuiTextFilter();
}

ContentExplorerPanel::~ContentExplorerPanel()
{
	delete m_TextFilter;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::OnAttach()
{
	Settings::SetDefaultInt("ContentExplorer", "ZoomLevel", 0);
	Settings::SetDefaultInt("ContentExplorer", "SortingMode", 0);
	m_ZoomLevel = (ZoomLevel)Settings::GetInt("ContentExplorer", "ZoomLevel");
	m_SortingMode = (Sorting)Settings::GetInt("ContentExplorer", "SortingMode");

	m_FileWatcher.Start([this](std::string path, FileStatus status)
		{
			m_ForceRescan = true;
		});
}

void ContentExplorerPanel::OnDetach()
{
	m_TextFilter->Clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::OnUpdate(float deltaTime)
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ContentExplorerPanel::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	static std::string filter(m_TextFilter->InputBuf);
	static FileType typeFilter = m_TypeFilter;

	if (filter != m_TextFilter->InputBuf || typeFilter != m_TypeFilter)
	{
		m_ForceRescan = true;
		filter = m_TextFilter->InputBuf;
		typeFilter = m_TypeFilter;
	}

	if (m_ForceRescan)
	{
		m_ForceRescan = false;

		if (m_CurrentPath == "")
		{
			m_CurrentPath = std::filesystem::absolute(".");
			m_History.SwitchTo(m_CurrentPath);
		}

		//set the input buffer as the current path
		memset(m_CurrentPathInputBuffer, 0, sizeof(m_CurrentPathInputBuffer));
		for (int i = 0; i < m_CurrentPath.string().length(); i++)
		{
			m_CurrentPathInputBuffer[i] = m_CurrentPath.string()[i];
		}

		if (m_TextFilter->IsActive() || m_TypeFilter != FileType::UNKNOWN)
		{
			std::vector<std::string> wantedExtensions;
			if (m_TypeFilter != FileType::UNKNOWN)
				wantedExtensions = ViewerManager::GetExtensions(m_TypeFilter);
			m_Files = Directory::GetFilesRecursive(m_CurrentPath, wantedExtensions, std::vector<std::string>(), m_SortingMode);
			m_Dirs.clear();

			m_Files.erase(
				std::remove_if(m_Files.begin(), m_Files.end(), [&](std::filesystem::path& file) {return !m_TextFilter->PassFilter(file.filename().string().c_str()); }),
				m_Files.end());
		}
		else
		{
			m_Dirs = Directory::GetDirectories(m_CurrentPath, m_SortingMode);
			m_Files = Directory::GetFiles(m_CurrentPath, m_SortingMode);
		}

		ClearSelected();

		m_NumberSelected = 0;
		m_LastSelectedFile = -1;
		m_LastSelectedDir = -1;

		if (m_TextureLibrary.Size() > 100)
			m_TextureLibrary.Clear();
	}

	ImGui::SetNextWindowSize(ImVec2(640, 700), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(500, 40), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(ICON_FA_FOLDER_OPEN " Content Explorer", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}
		HandleKeyboardInputs();
		//HandleMouseInputs();

		if (ImGui::BeginPopupContextWindow("Right click menu"))
		{
			RightClickMenu();
			ImGui::EndPopup();
		}

		const ImGuiStyle& style = ImGui::GetStyle();
		ImVec4 dummyButtonColour(0.0f, 0.0f, 0.0f, 0.5f);

		//----------------------------------------------------------------------------------------------------
		// History ('<', '>', '^')
		bool historyBackClicked = false;
		bool historyForwardClicked = false;
		bool levelUpClicked = false;

		ImGui::PushID("historyDirectoriesID");
		{
			const bool historyCanGoBack = m_History.CanGoBack();
			const bool historyCanGoForward = m_History.CanGoForward();
			const bool historyCanGoUp = m_History.CanGoUp();

			if (!historyCanGoBack)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
			}
			else if (ImGui::IsWindowHovered())
			{
				historyBackClicked = ImGui::IsMouseClicked(3);
			}
			historyBackClicked |= ImGui::Button(ICON_FA_ARROW_LEFT);

			if (!historyCanGoBack)
			{
				ImGui::PopStyleColor(3);
			}

			if (!historyCanGoForward)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
			}
			else if (ImGui::IsWindowHovered())
			{
				historyForwardClicked = ImGui::IsMouseClicked(4);
			}
			ImGui::SameLine();
			historyForwardClicked |= ImGui::Button(ICON_FA_ARROW_RIGHT);

			if (!historyCanGoForward)
			{
				ImGui::PopStyleColor(3);
			}

			if (!historyCanGoUp)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
			}

			ImGui::SameLine();
			levelUpClicked = ImGui::Button(ICON_FA_ARROW_UP);

			if (!historyCanGoUp)
			{
				ImGui::PopStyleColor(3);
			}
		}
		ImGui::PopID();

		if (historyBackClicked || historyForwardClicked || levelUpClicked)
		{
			if (historyBackClicked)
			{
				if (m_History.GoBack())
					m_ForceRescan = true;
			}
			else if (historyForwardClicked)
			{
				if (m_History.GoForward())
					m_ForceRescan = true;
			}
			else if (levelUpClicked)
			{
				if (m_History.GoUp())
					m_ForceRescan = true;
			}

			m_CurrentPath = *m_History.GetCurrentFolder();
		}

		//----------------------------------------------------------------------------------------------------
		//Sorting mode combo
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 3.0f);
		ImGui::SameLine();
		if (ImGui::Combo("##Sorting Mode", (int*)&m_SortingMode,
			ICON_FA_ARROW_DOWN_A_Z "\tAlphabetical\0"
			ICON_FA_ARROW_DOWN_Z_A "\tAlphabetical Reverse\0"
			ICON_FA_ARROW_DOWN_1_9 "\tLast Modified\0"
			ICON_FA_ARROW_DOWN_9_1 "\tLast Modified Reverse\0"
			ICON_FA_ARROW_DOWN_WIDE_SHORT "\tSize\0"
			ICON_FA_ARROW_DOWN_SHORT_WIDE "\tSize Reverse\0"
			ICON_FA_SORT_DOWN "\tType\0"
			ICON_FA_SORT_UP "\tType Reverse"))
		{
			m_ForceRescan = true;
			Settings::SetInt("ContentExplorer", "SortingMode", (int)m_SortingMode);
		}
		//----------------------------------------------------------------------------------------------------
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 3.0f);
		ImGui::SameLine();
		if (ImGui::Combo("##Zoom Level", (int*)&m_ZoomLevel,
			ICON_FA_BARS "\tList\0"
			ICON_FA_GRIP "\tThumbnails\0"
			ICON_FA_LIST "\tDetails\0"))
		{
			m_ForceRescan = true;
			Settings::SetInt("ContentExplorer", "ZoomLevel", (int)m_ZoomLevel);
		}
		//----------------------------------------------------------------------------------------------------
		// Manual Location text entry

		// Edit Location CheckButton
		bool editlocationInputTextReturnPressed = false;

		ImGui::PushStyleColor(ImGuiCol_Button, m_EditLocationCheckButtonPressed ? dummyButtonColour : style.Colors[ImGuiCol_Button]);

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FOLDER))
		{
			m_EditLocationCheckButtonPressed = !m_EditLocationCheckButtonPressed;
		}

		bool editLocationButtonDown = false;
		if (ImGui::IsItemActive())
			editLocationButtonDown = true;

		static bool editLocationShouldHaveFocus = false;
		if (m_EditLocationCheckButtonPressed != editLocationShouldHaveFocus)
		{
			editLocationShouldHaveFocus = !editLocationShouldHaveFocus;
			if (editLocationShouldHaveFocus)
				ImGui::SetKeyboardFocusHere();
		}

		ImGui::PopStyleColor();

		//----------------------------------------------------------------------------------------------------
		// Manual path edit control
		if (m_EditLocationCheckButtonPressed)
		{
			ImGui::SameLine();
			editlocationInputTextReturnPressed = ImGui::InputText("##EditLocationInputText", m_CurrentPathInputBuffer, sizeof(m_CurrentPathInputBuffer),
				ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank);

			static bool once = false;
			if (!ImGui::IsItemActive() && editLocationShouldHaveFocus)
			{
				editlocationInputTextReturnPressed = once;
				once = true;
			}

			if (editlocationInputTextReturnPressed)
			{
				once = false;
				try
				{
					if (std::filesystem::exists(m_CurrentPathInputBuffer))
					{
						SwitchTo(m_CurrentPathInputBuffer);
					}
					else
					{
						m_ForceRescan = false;
					}
					if (!editLocationButtonDown)
						m_EditLocationCheckButtonPressed = false;
				}
				catch (const std::exception& e)
				{
					CLIENT_ERROR(e.what());
				}
			}

			ImGui::Dummy(ImVec2(0, 0));
		}

		//----------------------------------------------------------------------------------------------------
		// Split path control
		else
		{
			const int numTabs = (int)m_CurrentSplitPath.size();

			int pushpop = 0;

			for (int t = 0; t < numTabs; t++)
			{
				if (t == numTabs - 1)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
				}

				ImGui::PushID(t);
				ImGui::SameLine();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));
				const bool pressed = ImGui::Button(m_CurrentSplitPath[t].c_str());
				if (t != numTabs - 1)
				{
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetFontSize());
					if (ImGui::BeginCombo("", ICON_FA_ANGLE_RIGHT, ImGuiComboFlags_NoArrowButton))
					{
						std::filesystem::path path = m_CurrentSplitPath[0];

						if (path == path.root_name())
						{
							path += std::filesystem::path::preferred_separator;
						}
						for (int i = 1; i <= t; i++)
						{
							path /= m_CurrentSplitPath[i];
						}

						auto directories = Directory::GetDirectories(path, m_SortingMode);

						for (int n = 0; n < directories.size(); n++)
						{
							std::filesystem::path directory = directories[n].filename();
							const bool is_selected = false;
							if (ImGui::Selectable(directory.string().c_str(), is_selected))
							{
								m_History.SwitchTo(directories[n]);
								m_ForceRescan = true;

								m_CurrentPath = *m_History.GetCurrentFolder();
								break;
							}
						}
						ImGui::EndCombo();
					}
				}
				ImGui::PopStyleVar();
				ImGui::PopID();

				if (pressed)
				{
					if (t == numTabs - 1)
					{
						m_EditLocationCheckButtonPressed = true;
						ImGui::PopStyleColor(3);
						continue;
					}

					m_History.SwitchTo(GetPathForSplitPathIndex(t));
					m_ForceRescan = true;

					m_CurrentPath = *m_History.GetCurrentFolder();
				}
				if (t == numTabs - 1)
				{
					ImGui::PopStyleColor(3);
				}
			}

			m_CurrentSplitPath = SplitString(m_CurrentPath.string(), std::filesystem::path::preferred_separator);

			ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
		}

		ImGui::Separator();
		// Search bar ---------------------------------------------------------------------------------------
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 3.0f);
		if (ImGui::Combo("##Type Filter", (int*)&m_TypeFilter,
			ICON_FA_FILTER "\tNone\0"
			ICON_FA_FILE_LINES "\tText\0"
			ICON_FA_FILE_IMAGE "\tImage\0"
			ICON_FA_SHAPES "\tMesh\0"
			ICON_FA_IMAGE "\tScene\0"
			ICON_FA_FILE_CODE "\tScript\0"
			ICON_FA_DIAGRAM_PROJECT "\tVisual Script\0"
			ICON_FA_FILE_AUDIO "\tAudio\0"
			ICON_FA_BOWLING_BALL "\tMaterial\0"
			ICON_FA_GRIP "\tTileset\0"
			ICON_FA_PHOTO_FILM "\tSprite Sheet\0"
			ICON_FA_VOLLEYBALL "\tPhysics Material\0"
			ICON_FA_FONT "\tFont\0"))
		{
		}

		ImGui::SameLine();
		ImGui::TextUnformatted(ICON_FA_MAGNIFYING_GLASS);
		ImGui::SameLine();
		m_TextFilter->Draw("##Search", ImGui::GetContentRegionAvail().x);
		ImGui::Tooltip("Filter (\"incl,-excl\")");
		ImGui::Separator();
		//----------------------------------------------------------------------------------------------------
		// MAIN BROWSING WINDOW
		//----------------------------------------------------------------------------------------------------
		{
			m_TotalNumBrowsingEntries = (uint32_t)(m_Dirs.size() + m_Files.size());

			static int id;
			ImGui::PushID(&id);

			switch (m_ZoomLevel)
			{
			case ContentExplorerPanel::ZoomLevel::List:
			{
				CalculateBrowsingDataTableSizes(ImGui::GetWindowSize());
				ImGui::Columns(m_NumBrowsingColumns);
				int cntEntries = 0;
				//Directories -------------------------------------------------------------------------------
				for (size_t i = 0; i < m_Dirs.size(); i++)
				{
					std::string dirName = ICON_FA_FOLDER " " + SplitString(m_Dirs[i].string(), std::filesystem::path::preferred_separator).back();
					if (ImGui::Selectable(dirName.c_str(), m_SelectedDirs[i], ImGuiSelectableFlags_AllowDoubleClick))
					{
						if (ImGui::GetIO().KeyCtrl)
						{
							m_NumberSelected -= m_SelectedDirs[i];
							m_SelectedDirs[i] = !m_SelectedDirs[i];

							m_NumberSelected += m_SelectedDirs[i];
						}
						else if (ImGui::GetIO().KeyShift)
						{
							if (m_LastSelectedDir != -1)
							{
								for (int j = 0; j < m_Dirs.size(); j++)
								{
									m_SelectedDirs[j] = (j >= std::min(m_LastSelectedDir, (int)i) && j <= std::max(m_LastSelectedDir, (int)i));
								}
							}
							else if (m_LastSelectedFile != -1)
							{
								for (int j = 0; j < m_Dirs.size(); j++)
								{
									m_SelectedDirs[j] = j >= i;
								}
								for (int j = 0; j < m_Files.size(); j++)
								{
									m_SelectedFiles[j] = j <= m_LastSelectedFile;
								}
							}
							m_NumberSelected = 2;
						}
						else
						{
							ClearSelected();

							m_SelectedDirs[i] = true;

							m_NumberSelected = 1;
							m_CurrentSelectedPath = m_Dirs[i];
						}

						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							//change directory on click
							m_History.SwitchTo(m_Dirs[i]);
							m_CurrentPath = *m_History.GetCurrentFolder();
							m_ForceRescan = true;
						}
						m_LastSelectedDir = (int)i;
						m_LastSelectedFile = -1;
					}

					ItemContextMenu(i, true, dirName);

					++cntEntries;
					if (cntEntries == m_NumBrowsingEntriesPerColumn)
					{
						cntEntries = 0;
						ImGui::NextColumn();
					}
				}

				//Files -----------------------------------------------------------------------------------
				for (int i = 0; i < m_Files.size(); i++)
				{
					if (!std::filesystem::exists(m_Files[i]))
					{
						m_Files.erase(m_Files.begin() + i);
						i--;
						continue;
					}
					ImGui::BeginGroup();

					std::string filename = GetFileIconForFileType(m_Files[i]) + " " + m_Files[i].filename().string();

					ImGui::PushID(i);

					if (ImGui::Selectable(filename.c_str(), m_SelectedFiles[i], ImGuiSelectableFlags_AllowDoubleClick))
					{
						if (ImGui::GetIO().KeyCtrl)
						{
							m_NumberSelected -= m_SelectedFiles[i];

							m_SelectedFiles[i] = !m_SelectedFiles[i];

							m_NumberSelected += m_SelectedFiles[i];

							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								OpenAllSelectedItems();
							}
						}
						else if (ImGui::GetIO().KeyShift)
						{
							if (m_LastSelectedFile != -1)
							{
								for (int j = 0; j < m_Files.size(); j++)
								{
									m_SelectedFiles[j] = (j >= std::min(m_LastSelectedFile, (int)i) && j <= std::max(m_LastSelectedFile, (int)i));
								}
							}
							else if (m_LastSelectedDir != -1)
							{
								for (int j = 0; j < m_Dirs.size(); j++)
								{
									m_SelectedDirs[j] = j >= m_LastSelectedDir;
								}
								for (int j = 0; j < m_Files.size(); j++)
								{
									m_SelectedFiles[j] = j <= i;
								}
							}
							m_NumberSelected = 2;
						}
						else
						{
							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								OpenItem(i);
							}

							ClearSelected();

							m_SelectedFiles[i] = true;

							m_NumberSelected = 1;

							m_CurrentSelectedPath = m_Files[i];
						}
						m_LastSelectedFile = i;
						m_LastSelectedDir = -1;
					}

					ItemContextMenu(i, false, filename);
					CreateDragDropSource(i);

					ImGui::EndGroup();

					++cntEntries;
					if (cntEntries == m_NumBrowsingEntriesPerColumn)
					{
						cntEntries = 0;
						ImGui::NextColumn();
					}
					ImGui::PopID();
				}
				break;
			}
			case ContentExplorerPanel::ZoomLevel::Thumbnails:
			{
				float thumbnailSize = 128;
				ImGuiTableFlags table_flags =
					ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY;

				float width = ImGui::GetWindowSize().x;

				ImGuiStyle& style = ImGui::GetStyle();

				m_NumBrowsingColumns = (int)(width / (thumbnailSize + (style.FramePadding.x * 2)));

				if (m_NumBrowsingColumns == 0)
					m_NumBrowsingColumns = 1;

				if (ImGui::BeginTable("Thumbnails Table", m_NumBrowsingColumns, table_flags))
				{
					if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(4))
					{
						if (m_History.GoForward())
							m_ForceRescan = true;
					}

					if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(3))
					{
						if (m_History.GoBack())
							m_ForceRescan = true;
					}

					m_CurrentPath = *m_History.GetCurrentFolder();

					ImGui::TableNextRow();

					//Directories -------------------------------------------------------------------------------
					for (size_t i = 0; i < m_Dirs.size(); i++)
					{
						ImGui::TableNextColumn();
						std::string dirName = SplitString(m_Dirs[i].string(), std::filesystem::path::preferred_separator).back();
						ImGui::BeginGroup();

						ImGui::PushFont(Fonts::Icons);
						if (ImGui::Button(ICON_FA_FOLDER_OPEN, { thumbnailSize, thumbnailSize }))
						{
							if (!ImGui::GetIO().KeyCtrl)
							{
								ClearSelected();

								m_SelectedDirs[i] = true;

								m_NumberSelected = 1;
								m_CurrentSelectedPath = m_Dirs[i];
							}
							else
							{
								m_NumberSelected -= m_SelectedDirs[i];
								m_SelectedDirs[i] = !m_SelectedDirs[i];

								m_NumberSelected += m_SelectedDirs[i];
							}
						}
						ImGui::PopFont();
						ImGui::TextWrapped("%s", dirName.c_str());

						ImGui::EndGroup();
						ItemContextMenu(i, true, dirName);

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							//change directory on click
							m_History.SwitchTo(m_Dirs[i]);
							m_CurrentPath = *m_History.GetCurrentFolder();
							m_ForceRescan = true;
						}
						m_LastSelectedDir = (int)i;
						m_LastSelectedFile = -1;
					}

					//Files -------------------------------------------------------------------------------
					for (size_t i = 0; i < m_Files.size(); i++)
					{
						ImGui::TableNextColumn();
						std::string filename = m_Files[i].filename().string();

						ImGui::BeginGroup();
						ImGui::PushID((int)i);
						ImGui::PushFont(Fonts::Icons);

						// try to get the image to display the thumbnail
						if (ViewerManager::GetFileType(m_Files[i]) == FileType::IMAGE)
						{
							Ref<Texture2D> icon = m_TextureLibrary.Load(m_Files[i], nullptr);
							uint32_t textureHeight = icon->GetHeight();
							uint32_t textureWidth = icon->GetWidth();

							if (textureHeight == textureWidth)
							{
								ImGui::ImageButton(icon, { thumbnailSize, thumbnailSize });
							}
							else if (textureHeight < textureWidth)
							{
								ImGui::BeginGroup();
								ImGui::PushID((int)i);
								float aspectRatio = (float)textureHeight / (float)textureWidth;
								ImGui::Dummy({ 0.0f, ((1.0f - aspectRatio) * thumbnailSize / 2.0f) - 0.5f * (textureWidth / thumbnailSize) });
								ImGui::ImageButton(icon, ImVec2(thumbnailSize, aspectRatio * thumbnailSize));
								ImGui::Dummy({ 0.0f, ((1.0f - aspectRatio) * thumbnailSize / 2.0f) - 0.5f * (textureWidth / thumbnailSize) });
								ImGui::EndGroup();
								ImGui::PopID();
							}
							else
							{
								ImGui::BeginGroup();
								ImGui::PushID((int)i);
								float aspectRatio = (float)textureWidth / (float)textureHeight;
								ImGui::Dummy({ ((1.0f - aspectRatio) * thumbnailSize / 2.0f) - 0.5f * ((float)textureHeight / thumbnailSize), 0.0f });
								ImGui::SameLine();
								ImGui::ImageButton(icon, ImVec2(aspectRatio * thumbnailSize, thumbnailSize));
								ImGui::SameLine();
								ImGui::Dummy({ ((1.0f - aspectRatio) * thumbnailSize / 2.0f) - 0.5f * ((float)textureHeight / thumbnailSize), 0.0f });
								ImGui::EndGroup();
								ImGui::PopID();
							}
						}
						else
							ImGui::Button(GetFileIconForFileType(m_Files[i]).c_str(), { thumbnailSize, thumbnailSize });

						if ((ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
						{
							if (!ImGui::GetIO().KeyCtrl)
							{
								ClearSelected();

								m_SelectedFiles[i] = true;

								m_NumberSelected = 1;

								m_CurrentSelectedPath = m_Files[i];
							}
							else
							{
								m_NumberSelected -= m_SelectedFiles[i];

								m_SelectedFiles[i] = !m_SelectedFiles[i];

								m_NumberSelected += m_SelectedFiles[i];
							}
							m_LastSelectedFile = (int)i;
							m_LastSelectedDir = -1;
						}
						ImGui::PopFont();
						ItemContextMenu(i, false, filename);
						CreateDragDropSource(i);
						ImGui::TextWrapped("%s", filename.c_str());
						ImGui::EndGroup();
						ImGui::PopID();
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							OpenAllSelectedItems();
						}
					}

					if (ImGui::BeginPopupContextWindow("Right click menu",
						ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_MouseButtonRight))
					{
						RightClickMenu();
						ImGui::EndPopup();
					}

					ImGui::EndTable();
				}
			}
			break;
			case ContentExplorerPanel::ZoomLevel::Details:
			{
				ImGuiTableFlags table_flags =
					ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
					| ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
					| ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY;

				if (ImGui::BeginTable("Details Table", 3, table_flags))
				{
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort, 0.0f, 0);
					ImGui::TableSetupColumn("Date Modified", ImGuiTableColumnFlags_None, 0.0f, 1);
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_None, 0.0f, 2);
					ImGui::TableSetupScrollFreeze(0, 1);

					if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
					{
						if (sort_specs->SpecsDirty)
						{
							m_ForceRescan = true;
							for (int n = 0; n < sort_specs->SpecsCount; n++)
							{
								const ImGuiTableColumnSortSpecs* sort_spec = &sort_specs->Specs[n];
								if (sort_spec->SortDirection == ImGuiSortDirection_Ascending)
								{
									switch (sort_spec->ColumnUserID)
									{
									case 0: m_SortingMode = Sorting::ALPHABETIC; break;
									case 1: m_SortingMode = Sorting::LAST_MODIFICATION; break;
									case 2: m_SortingMode = Sorting::SIZE; break;
									default: break;
									}
								}
								else if (sort_spec->SortDirection == ImGuiSortDirection_Descending)
								{
									switch (sort_spec->ColumnUserID)
									{
									case 0: m_SortingMode = Sorting::ALPHABETIC_INVERSE; break;
									case 1: m_SortingMode = Sorting::LAST_MODIFICATION_INVERSE; break;
									case 2: m_SortingMode = Sorting::SIZE_INVERSE; break;
									default: break;
									}
								}
							}
							sort_specs->SpecsDirty = false;
						}
					}

					ImGui::TableHeadersRow();

					//Forward button
					if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(4))
					{
						if (m_History.GoForward())
							m_ForceRescan = true;
					}

					//Back button
					if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(3))
					{
						if (m_History.GoBack())
							m_ForceRescan = true;
					}

					m_CurrentPath = *m_History.GetCurrentFolder();

					ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns
						| ImGuiSelectableFlags_AllowItemOverlap
						| ImGuiSelectableFlags_AllowDoubleClick;

					//Directories -------------------------------------------------------------------------------
					for (size_t i = 0; i < m_Dirs.size(); i++)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string dirName = ICON_FA_FOLDER " " + SplitString(m_Dirs[i].string(), std::filesystem::path::preferred_separator).back();
						if (ImGui::Selectable(dirName.c_str(), m_SelectedDirs[i], selectable_flags))
						{
							if (!ImGui::GetIO().KeyCtrl)
							{
								ClearSelected();

								m_SelectedDirs[i] = true;

								m_NumberSelected = 1;
								m_CurrentSelectedPath = m_Dirs[i];
							}
							else
							{
								m_NumberSelected -= m_SelectedDirs[i];
								m_SelectedDirs[i] = !m_SelectedDirs[i];

								m_NumberSelected += m_SelectedDirs[i];
							}

							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								//change directory on click
								m_History.SwitchTo(m_Dirs[i]);
								m_CurrentPath = *m_History.GetCurrentFolder();
								m_ForceRescan = true;
							}
							m_LastSelectedDir = (int)i;
							m_LastSelectedFile = -1;
						}

						ItemContextMenu(i, true, dirName);

						ImGui::TableSetColumnIndex(1);
						if (!m_ForceRescan)
						{
							std::time_t cftime = to_time_t<std::filesystem::file_time_type>(std::filesystem::last_write_time(m_Dirs[i]));

							char buff[20];
							strftime(buff, 20, "%d/%m/%Y %H:%M:%S", localtime(&cftime));

							ImGui::Text("%s", buff);
						}
					}

					//Files -----------------------------------------------------------------------------------
					for (size_t i = 0; i < m_Files.size(); i++)
					{
						if (!std::filesystem::exists(m_Files[i]))
						{
							m_Files.erase(m_Files.begin() + i);
							i--;
							continue;
						}

						ImGui::BeginGroup();
						std::string filename = GetFileIconForFileType(m_Files[i]) + " " + m_Files[i].filename().string();

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);

						if (ImGui::Selectable(filename.c_str(), m_SelectedFiles[i], selectable_flags))
						{
							if (!ImGui::GetIO().KeyCtrl)
							{
								if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
								{
									OpenItem(i);
								}

								ClearSelected();

								m_SelectedFiles[i] = true;

								m_NumberSelected = 1;

								m_CurrentSelectedPath = m_Files[i];
							}
							else
							{
								m_NumberSelected -= m_SelectedFiles[i];

								m_SelectedFiles[i] = !m_SelectedFiles[i];

								m_NumberSelected += m_SelectedFiles[i];

								if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
								{
									OpenAllSelectedItems();
								}
							}
							m_LastSelectedFile = (int)i;
							m_LastSelectedDir = -1;
						}
						ItemContextMenu(i, false, filename);
						CreateDragDropSource(i);

						ImGui::TableSetColumnIndex(1);
						if (!m_ForceRescan)
						{
							std::time_t cftime = to_time_t<std::filesystem::file_time_type>(std::filesystem::last_write_time(m_Files[i]));

							char buff[20];
							strftime(buff, 20, "%d/%m/%Y %H:%M:%S", std::localtime(&cftime));

							ImGui::Text("%s", buff);
						}

						ImGui::TableSetColumnIndex(2);
						if (!m_ForceRescan)
						{
							ImGui::Text("%iKB", (int)ceil(std::filesystem::file_size(m_Files[i]) / 1000.0f));
						}

						ImGui::EndGroup();
					}

					if (ImGui::BeginPopupContextWindow("Right click menu",
						ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_MouseButtonRight))
					{
						RightClickMenu();
						ImGui::EndPopup();
					}

					ImGui::EndTable();
				}
				break;
			}
			default:
				break;
			}

			ImGui::PopID();
		}

		if (m_Renaming) {
			ImGui::OpenPopup("Rename");
		}

		if (ImGui::BeginPopup("Rename"))
		{
			Rename();
			ImGui::EndPopup();
		}

		if (m_TryingToChangeScene) {
			if (SceneManager::CurrentScene()->IsDirty()) {
				ImGui::OpenPopup("Save Scene?");
			}
			else {
				SceneManager::ChangeScene(m_CurrentSelectedPath);
				m_TryingToChangeScene = false;
			}
		}

		if (ImGui::BeginPopupModal("Save Scene?"))
		{
			ImGui::TextUnformatted("Save unsaved changes in Scene?");
			if (ImGui::Button("Save"))
			{
				SceneManager::CurrentScene()->Save();
				OpenAllSelectedItems();
				m_TryingToChangeScene = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				SceneManager::CurrentScene()->MakeClean();
				SceneManager::ChangeScene(m_CurrentSelectedPath);
				OpenAllSelectedItems();
				m_TryingToChangeScene = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_TryingToChangeScene = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void ContentExplorerPanel::Copy()
{
	m_CopiedPaths.clear();
	for (size_t i = 0; i < m_SelectedFiles.size(); i++)
	{
		if (m_SelectedFiles[i])
			m_CopiedPaths.push_back(m_Files[i]);
	}
}

void ContentExplorerPanel::Cut()
{
	m_Cut = true;
	Copy();
}