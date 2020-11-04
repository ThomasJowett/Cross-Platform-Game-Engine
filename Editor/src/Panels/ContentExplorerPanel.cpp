#include "ContentExplorerPanel.h"

#include "imgui/imgui.h"

#include "Engine.h"

#include "Viewers/ViewerManager.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome5.h"

#include "FileSystem/FileDialog.h"

#include "Importers/ImportManager.h"

void ContentExplorerPanel::Paste()
{
	for (auto path : m_CopiedPaths)
	{
		auto target = m_CurrentPath / path.filename();

		if (std::filesystem::exists(target))
		{
			int suffix = 1;
			std::string extenstion = target.extension().string();

			std::filesystem::path targetNoExt = target;
			targetNoExt.replace_extension("");

			std::filesystem::path temp = targetNoExt.string() + "- Copy" + extenstion;

			while (std::filesystem::exists(temp))
			{
				suffix++;
				temp = targetNoExt.string() + "- Copy (" + std::to_string(suffix) + ')' + extenstion;
			}

			target = temp;
		}

		try
		{
			std::filesystem::copy_file(path, target, std::filesystem::copy_options::skip_existing);
		}
		catch (std::exception &e)
		{
			CLIENT_ERROR(e.what());
		}
	}
	m_ForceRescan = true;
}

void ContentExplorerPanel::Duplicate()
{
	Copy();
	Paste();
}

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

void ContentExplorerPanel::SelectAll()
{
	for (auto file : m_SelectedFiles)
		file = true;

	for (auto dir : m_SelectedDirs)
		dir = true;
}

bool ContentExplorerPanel::HasSelection() const
{
	return m_NumberSelected > 0;
}

bool ContentExplorerPanel::Rename()
{
	static char inputBuffer[1024] = "";

	memset(inputBuffer, 0, sizeof(inputBuffer));
	for (int i = 0; i < m_CurrentSelectedPath.filename().string().length(); i++)
	{
		inputBuffer[i] = m_CurrentSelectedPath.filename().string()[i];
	}
	static bool hasFocus = false;
	static bool once = false;

	if (!hasFocus)
	{
		ImGui::SetKeyboardFocusHere();
		hasFocus = true;
	}

	if (ImGui::InputText("##RenameBox", inputBuffer, sizeof(inputBuffer),
						 ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (!std::filesystem::exists(m_CurrentPath / inputBuffer))
		{
			std::filesystem::rename(m_CurrentSelectedPath, m_CurrentPath / inputBuffer);
			m_ForceRescan = true;
		}
		else
		{
			CLIENT_ERROR("Could not name folder: Folder already exists!");
		}

		ImGui::CloseCurrentPopup();
		hasFocus = false;
		once = false;
		return true;
	}

	if (!ImGui::IsItemActive())
	{
		if (once)
		{
			ImGui::CloseCurrentPopup();
			hasFocus = false;
			once = false;
			return false;
		}
		once = true;
	}
	return false;
}

void ContentExplorerPanel::SwitchTo(const std::filesystem::path &path)
{
	m_CurrentPath = path;

	if (std::filesystem::is_regular_file(m_CurrentPath))
	{
		m_CurrentPath.remove_filename();
		std::string str = m_CurrentPath.string();
		str.pop_back();
		m_CurrentPath = str;
	}

	m_CurrentSplitPath = SplitString(m_CurrentPath.string(), '\\');
	m_History.SwitchTo(m_CurrentPath);
	m_ForceRescan = true;
}

void ContentExplorerPanel::CreateNewScene() //TODO: create a pop-up to name the scene before creating it
{
	std::string newSceneFilepath = m_CurrentPath.string() + "\\Untitled";

	int suffix = 1;

	if (std::filesystem::exists(newSceneFilepath + ".scene"))
	{
		while (std::filesystem::exists(newSceneFilepath + '(' + std::to_string(suffix) + ").scene"))
		{
			suffix++;
		}

		newSceneFilepath += '(' + std::to_string(suffix) + ')';
	}

	newSceneFilepath += ".scene";

	Ref<Scene> newScene = CreateRef<Scene>(newSceneFilepath);

	newScene->Save(false);

	m_ForceRescan = true;

	m_CurrentSelectedPath = newSceneFilepath;
}

std::filesystem::path ContentExplorerPanel::GetPathForSplitPathIndex(int index)
{
	std::string path;
	for (int i = 0; i <= index; i++)
	{
		path += m_CurrentSplitPath[i];
		if (i != index)
			path += '\\';
	}

	return std::filesystem::path(path);
}

void ContentExplorerPanel::CalculateBrowsingDataTableSizes(const ImVec2 &childWindowSize)
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

void ContentExplorerPanel::HandleKeyboardInputs()
{
	ImGuiIO &io = ImGui::GetIO();
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
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed('D'))
			Duplicate();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
			SelectAll();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed('R') && m_NumberSelected == 1)
			ImGui::OpenPopup("Rename");
	}
}

void ContentExplorerPanel::RightClickMenu()
{
	ImGuiStyle *style = &ImGui::GetStyle();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
	if (ImGui::BeginMenu("New"))
	{
		if (ImGui::SmallButton("Folder"))
		{
			std::string newFolderName = m_CurrentPath.string() + "\\New folder";
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

			ImGui::OpenPopup("Rename");

			m_CurrentSelectedPath = newFolderName;
		}
		if (ImGui::SmallButton("Scene"))
		{
			CreateNewScene();
			ImGui::OpenPopup("Rename");
		}
		if (ImGui::SmallButton("Object"))
			CLIENT_DEBUG("new object");

		if (ImGui::BeginPopup("Rename"))
		{
			Rename();
			ImGui::EndPopup();
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Import Assets"))
	{
		std::optional<std::vector<std::wstring>> assetPaths = FileDialog::MultiOpen(L"Select Files...",
																					L"Any File\0*.*\0"
																					"Film Box (.fbx)\0*.fbx\0"
																					"Wavefront OBJ (.obj)\0*.obj");

		if (assetPaths)
		{
			ImportManager::ImportMultiAssets(assetPaths.value(), m_CurrentPath);
			m_ForceRescan = true;
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Cut", "Ctrl + X", nullptr, m_NumberSelected > 0))
		Cut();
	if (ImGui::MenuItem("Copy", "Ctrl + C", nullptr, m_NumberSelected > 0))
		Copy();
	if (ImGui::MenuItem("Paste", "Ctrl + V", nullptr, m_CopiedPaths.size() > 0))
		Paste();
	if (ImGui::MenuItem("Duplicate", "Ctrl + D", nullptr, m_NumberSelected > 0))
		Duplicate();
	if (ImGui::MenuItem("Delete", "Del", nullptr, m_NumberSelected > 0))
		Delete();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void ContentExplorerPanel::OpenAllSelectedItems()
{
	for (size_t i = 0; i < m_Files.size(); i++)
	{
		if (m_SelectedFiles[i])
			ViewerManager::OpenViewer(m_Files[i]);
	}
}

ContentExplorerPanel::ContentExplorerPanel(bool *show)
	: m_Show(show), Layer("ContentExplorer")
{
	m_TotalNumBrowsingEntries = 0;
	m_NumBrowsingColumns = 0;
	m_NumBrowsingEntriesPerColumn = 0;
	m_NumberSelected = 0;
}

void ContentExplorerPanel::OnAttach()
{
}

void ContentExplorerPanel::OnUpdate(float deltaTime)
{
}

void ContentExplorerPanel::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	static char inputBuffer[1024] = "";

	if (m_ForceRescan)
	{
		m_ForceRescan = false;

		if (m_CurrentPath == "")
		{
			m_CurrentPath = std::filesystem::absolute(".");
			m_History.SwitchTo(m_CurrentPath);
		}

		//set the input buffer as the current path
		memset(inputBuffer, 0, sizeof(inputBuffer));
		for (int i = 0; i < m_CurrentPath.string().length(); i++)
		{
			inputBuffer[i] = m_CurrentPath.string()[i];
		}

		m_Dirs = Directory::GetDirectories(m_CurrentPath, m_SortingMode);
		m_Files = Directory::GetFiles(m_CurrentPath, m_SortingMode);

		m_SelectedDirs.clear();
		m_SelectedDirs.resize(m_Dirs.size());
		m_SelectedFiles.clear();
		m_SelectedFiles.resize(m_Files.size());

		m_NumberSelected = 0;
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

		if (ImGui::BeginPopupContextWindow("Right click menu", 1, false))
		{
			RightClickMenu();
			ImGui::EndPopup();
		}

		//ImGui::SetNextWindowPos(m_CurrentSelectedPosition);
		if (ImGui::BeginPopup("Rename"))
		{
			Rename();
			ImGui::EndPopup();
		}
		const ImGuiStyle &style = ImGui::GetStyle();
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
		if (ImGui::Combo("##Sorting Mode", (int *)&m_SortingMode,
						 ICON_FA_SORT_ALPHA_DOWN "\tAlphabetical\0" 
						 ICON_FA_SORT_ALPHA_DOWN_ALT "\tAlphabetical Reverse\0" 
						 ICON_FA_SORT_NUMERIC_DOWN_ALT "\tLast Modified\0" 
						 ICON_FA_SORT_NUMERIC_DOWN "\tLast Modified Reverse\0" 
						 ICON_FA_SORT_AMOUNT_DOWN_ALT "\tSize\0" 
						 ICON_FA_SORT_AMOUNT_DOWN "\tSize Reverse\0" 
						 ICON_FA_SORT_DOWN "\tType\0" 
						 ICON_FA_SORT_UP "\tType Reverse"))
			m_ForceRescan = true;
		//----------------------------------------------------------------------------------------------------
		// Manual Location text entry
		const std::filesystem::path *fi = m_History.GetCurrentFolder();

		// Edit Location CheckButton
		bool editlocationInputTextReturnPressed = false;
		{
			bool mustValidateInputPath = false;
			ImGui::PushStyleColor(ImGuiCol_Button, m_EditLocationCheckButtonPressed ? dummyButtonColour : style.Colors[ImGuiCol_Button]);

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_FOLDER))
			{
				m_EditLocationCheckButtonPressed = !m_EditLocationCheckButtonPressed;
			}

			static bool hasFocus = false;
			if (m_EditLocationCheckButtonPressed != hasFocus)
			{
				hasFocus = !hasFocus;
				if (hasFocus)
					ImGui::SetKeyboardFocusHere();
			}

			ImGui::PopStyleColor();

			if (m_EditLocationCheckButtonPressed)
			{
				ImGui::SameLine();
				editlocationInputTextReturnPressed = ImGui::InputText("##EditLocationInputText", inputBuffer, sizeof(inputBuffer),
																	  ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank);

				static bool once = false;
				if (!ImGui::IsItemActive())
				{
					editlocationInputTextReturnPressed = once;
					once = true;
				}

				if (editlocationInputTextReturnPressed)
				{
					once = false;
					mustValidateInputPath = true;
				}
			}

			if (mustValidateInputPath)
			{
				try
				{
					if (std::filesystem::exists(inputBuffer))
					{
						SwitchTo(inputBuffer);
					}
					else
					{
						m_ForceRescan = false;
					}
					m_EditLocationCheckButtonPressed = false;
				}
				catch (const std::exception &e)
				{
					CLIENT_ERROR(e.what());
				}
			}
		}
		//----------------------------------------------------------------------------------------------------
		// Split path control
		if (!m_EditLocationCheckButtonPressed && !editlocationInputTextReturnPressed)
		{
			bool mustSwitchSplitPath = false;

			//Split Path
			// tab:
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
							std::string path;
							for (int i = 0; i <= t; i++)
							{
								path += m_CurrentSplitPath[i];
								path += '\\';
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

				m_CurrentSplitPath = SplitString(m_CurrentPath.string(), '\\');
			}
		}

		ImGui::Separator();
		//----------------------------------------------------------------------------------------------------
		// MAIN BROWSING WINDOW
		//----------------------------------------------------------------------------------------------------
		{
			m_TotalNumBrowsingEntries = (uint32_t)(m_Dirs.size() + m_Files.size());
			CalculateBrowsingDataTableSizes(ImGui::GetWindowSize());

			ImGui::Columns(m_NumBrowsingColumns);

			static int id;
			ImGui::PushID(&id);
			int cntEntries = 0;
			//Directories -------------------------------------------------------------------------------
			if (m_Dirs.size() > 0)
			{
				for (int i = 0; i < m_Dirs.size(); i++)
				{
					std::string dirName = ICON_FA_FOLDER " " + SplitString(m_Dirs[i].string(), '\\').back();
					if (ImGui::Selectable(dirName.c_str(), m_SelectedDirs[i], ImGuiSelectableFlags_AllowDoubleClick))
					{
						if (!ImGui::GetIO().KeyCtrl)
						{
							m_SelectedDirs.clear();
							m_SelectedDirs.resize(m_Dirs.size());

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

						if (ImGui::IsMouseDoubleClicked(0))
						{
							//change directory on click
							m_History.SwitchTo(m_Dirs[i]);
							m_CurrentPath = *m_History.GetCurrentFolder();
							m_ForceRescan = true;
						}
					}

					if (ImGui::BeginPopupContextItem(dirName.c_str()))
					{
						if (!m_SelectedDirs[i])
						{
							m_SelectedDirs.clear();
							m_SelectedDirs.resize(m_Dirs.size());

							m_SelectedDirs[i] = true;

							m_NumberSelected = 1;
						}

						m_CurrentSelectedPath = m_Dirs[i];

						if (ImGui::Button("Rename", ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetFontSize())) && m_NumberSelected == 1)
						{
							ImGui::OpenPopup("Rename");
						}
						if (ImGui::BeginPopup("Rename"))
						{
							Rename();
							ImGui::EndPopup();
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Cut", "Ctrl + X", nullptr, m_NumberSelected > 0))
							Cut();
						if (ImGui::MenuItem("Copy", "Ctrl + C", nullptr, m_NumberSelected > 0))
							Copy();
						if (ImGui::MenuItem("Paste", "Ctrl + V", nullptr, m_CopiedPaths.size() > 0))
							Paste();
						if (ImGui::MenuItem("Duplicate", "Ctrl + D", nullptr, m_NumberSelected > 0))
							Duplicate();
						if (ImGui::MenuItem("Delete", "Del", nullptr, m_NumberSelected > 0))
							Delete();
						ImGui::EndPopup();
					}
					++cntEntries;
					//TODO:: switch on view zoom
					if (cntEntries == m_NumBrowsingEntriesPerColumn)
					{
						cntEntries = 0;
						ImGui::NextColumn();
					}
				}
			}

			//Files -----------------------------------------------------------------------------------
			if (m_Files.size() > 0)
			{
				for (int i = 0; i < m_Files.size(); i++)
				{
					if (!std::filesystem::exists(m_Files[i]))
					{
						m_Files.erase(m_Files.begin() + i);
						i--;
						continue;
					}
					ImGui::BeginGroup();

					std::string filename = ICON_FA_FILE " " + m_Files[i].filename().string();

					if (ImGui::Selectable(filename.c_str(), m_SelectedFiles[i], ImGuiSelectableFlags_AllowDoubleClick))
					{
						if (!ImGui::GetIO().KeyCtrl)
						{
							if (ImGui::IsMouseDoubleClicked(0))
							{
								ViewerManager::OpenViewer(m_Files[i]);
							}

							m_SelectedFiles.clear();
							m_SelectedFiles.resize(m_Files.size());

							m_SelectedFiles[i] = true;

							m_NumberSelected = 1;

							m_CurrentSelectedPosition = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPos().y);
							m_CurrentSelectedPath = m_Files[i];
						}
						else
						{
							m_NumberSelected -= m_SelectedFiles[i];

							m_SelectedFiles[i] = !m_SelectedFiles[i];

							m_NumberSelected += m_SelectedFiles[i];

							if (ImGui::IsMouseDoubleClicked(0))
							{
								OpenAllSelectedItems();
							}
						}
					}

					if (ImGui::BeginPopupContextItem(filename.c_str()))
					{
						// if the file that this context menu is for is not selected then select it and unselect all that is selected
						if (!m_SelectedFiles[i])
						{
							m_SelectedFiles.clear();
							m_SelectedFiles.resize(m_Files.size());

							m_SelectedFiles[i] = true;

							m_NumberSelected = 1;
						}

						m_CurrentSelectedPosition = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPos().y);
						m_CurrentSelectedPath = m_Files[i];

						if (ImGui::Button("Rename", ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetFontSize())) && m_NumberSelected == 1)
						{
							ImGui::OpenPopup("Rename");
						}
						if (ImGui::BeginPopup("Rename"))
						{
							Rename();
							ImGui::EndPopup();
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Cut", "Ctrl + X", nullptr, m_NumberSelected > 0))
							Cut();
						if (ImGui::MenuItem("Copy", "Ctrl + C", nullptr, m_NumberSelected > 0))
							Copy();
						if (ImGui::MenuItem("Paste", "Ctrl + V", nullptr, m_CopiedPaths.size() > 0))
							Paste();
						if (ImGui::MenuItem("Duplicate", "Ctrl + D", nullptr, m_NumberSelected > 0))
							Duplicate();
						if (ImGui::MenuItem("Delete", "Del", nullptr, m_NumberSelected > 0))
							Delete();

						ImGui::EndPopup();
					}

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						ImGui::SetDragDropPayload("Asset", &m_Files[i], sizeof(std::filesystem::path));
						ImGui::Text("%s", m_Files[i].filename().string().c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::EndGroup();

					if (!m_ForceRescan)
					{
						ImGui::SameLine(std::max(ImGui::CalcTextSize(m_Files[i].filename().string().c_str()).x + 15, 300.0f));
						ImGui::Text("%s", (std::to_string((int)ceil(std::filesystem::file_size(m_Files[i]) / 1000.0f)) + "KB").c_str());
					}

					++cntEntries;
					//TODO:: switch on view zoom
					if (cntEntries == m_NumBrowsingEntriesPerColumn)
					{
						cntEntries = 0;
						ImGui::NextColumn();
					}
				}
			}

			ImGui::PopID();
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
	CLIENT_ERROR("ContentExplorerPanel::Cut() Function not implemented!");
}
