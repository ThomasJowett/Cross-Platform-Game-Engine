#include "ImGuiContentExplorer.h"

#include "imgui/imgui.h"

#include "Renderer/Texture.h"

#include "Viewers/ViewerManager.h"

std::filesystem::path ImGuiContentExplorer::GetPathForSplitPathIndex(int index)
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

void ImGuiContentExplorer::CalculateBrowsingDataTableSizes(const ImVec2& childWindowSize)
{
	int approxNumEntriesPerColumn = 20;
	if (childWindowSize.y > 0)
	{
		int numLinesThatFit = (int)(childWindowSize.y / ImGui::GetTextLineHeightWithSpacing());
		if (numLinesThatFit <= 0)
			numLinesThatFit = 1;
		approxNumEntriesPerColumn = numLinesThatFit;
	}
	m_NumBrowsingColumns = std::ceil((float)m_TotalNumBrowsingEntries / (float)approxNumEntriesPerColumn);

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


ImGuiContentExplorer::ImGuiContentExplorer(bool* show)
	:m_Show(show), Layer("ContentExplorer")
{
	m_TotalNumBrowsingEntries = 0;
	m_NumBrowsingColumns = 0;
	m_NumBrowsingEntriesPerColumn = 0;
}

void ImGuiContentExplorer::OnAttach()
{
	//TODO: Have an Icon manager or something that loads the correct size icon bitmaps
	//then have that texture built into the binary
	m_TextureLibrary.Load("resources/Icons/Folder16.png");
	m_TextureLibrary.Load("resources/Icons/Arrow_Right.png");
	m_TextureLibrary.Load("resources/Icons/Arrow_Left.png");
	m_TextureLibrary.Load("resources/Icons/Arrow_Up.png");
}

void ImGuiContentExplorer::OnUpdate(float deltaTime)
{
}

void ImGuiContentExplorer::OnImGuiRender()
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
		}

		//set the input buffer as the current path
		memset(inputBuffer, 0, sizeof(inputBuffer));
		for (int i = 0; i < m_CurrentPath.string().length(); i++)
		{
			inputBuffer[i] = m_CurrentPath.string()[i];
		}

		m_Dirs = Directory::GetDirectories(m_CurrentPath, m_SortingMode);
		m_Files = Directory::GetFiles(m_CurrentPath, m_SortingMode);
	}
	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Content Explorer", m_Show))
	{
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

			if (!historyCanGoBack)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
			}
			historyBackClicked = ImGui::ArrowButton("##BackButton", ImGuiDir_Left);

			if (!historyCanGoBack) {
				ImGui::PopStyleColor(3);
			}

			if (!historyCanGoForward)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
			}
			ImGui::SameLine();
			historyForwardClicked = ImGui::ArrowButton("##ForwardButton", ImGuiDir_Right);

			if (!historyCanGoForward) {
				ImGui::PopStyleColor(3);
			}
		}
		ImGui::PopID();

		if (historyBackClicked || historyForwardClicked)
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

			m_CurrentPath = *m_History.GetCurrentFolder();
		}
		//----------------------------------------------------------------------------------------------------
		// Manual Location text entry
		const std::filesystem::path* fi = m_History.GetCurrentFolder();

		// Edit Location CheckButton
		bool editlocationInputTextReturnPressed = false;
		{
			bool mustValidateInputPath = false;
			ImGui::PushStyleColor(ImGuiCol_Button, m_EditLocationCheckButtonPressed ? dummyButtonColour : style.Colors[ImGuiCol_Button]);
			
			ImGui::SameLine();
			if (ImGui::ImageButton(m_TextureLibrary.Get("resources/Icons/Folder16.png"), { 16,16 }, 0, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]))
			{
				m_EditLocationCheckButtonPressed = !m_EditLocationCheckButtonPressed;
				if (m_EditLocationCheckButtonPressed)
				{
					ImGui::SetKeyboardFocusHere();
				}
			}

			ImGui::PopStyleColor();


			if (m_EditLocationCheckButtonPressed)
			{
				ImGui::SameLine();
				editlocationInputTextReturnPressed = ImGui::InputText("##EditLocationInputText", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

				ImGui::SetKeyboardFocusHere();
				if (editlocationInputTextReturnPressed)
				{
					mustValidateInputPath = true;
				}
				else ImGui::Separator();
			}

			if (mustValidateInputPath)
			{
				try
				{
					if (std::filesystem::exists(inputBuffer))
					{
						m_CurrentPath = inputBuffer;

						if (std::filesystem::is_regular_file(inputBuffer))
						{
							m_CurrentPath.remove_filename();
							std::string path = m_CurrentPath.string();
							path.pop_back();
							m_CurrentPath = path;
						}
						m_CurrentSplitPath = SplitString(m_CurrentPath.string(), '\\');
						m_EditLocationCheckButtonPressed = false;
						m_ForceRescan = true;
						m_History.SwitchTo(m_CurrentPath);
					}
				}
				catch (const std::exception& e)
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

					const bool pressed = ImGui::Button(m_CurrentSplitPath[t].c_str());
					if (t != numTabs - 1)
					{
						ImGui::SameLine();
						ImGui::Text(">");
					}
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
					if (t == numTabs - 1) {
						ImGui::PopStyleColor(3);
					}
				}

				m_CurrentSplitPath = SplitString(m_CurrentPath.string(), '\\');

				ImGui::Separator();
			}
		}

		//----------------------------------------------------------------------------------------------------
		// MAIN BROWSING WINDOW
		//----------------------------------------------------------------------------------------------------
		{
			if (ImGui::BeginChild("BrowsingFrame"))
			{
				m_TotalNumBrowsingEntries = m_Dirs.size() + m_Files.size();
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
						std::string dirName = SplitString(m_Dirs[i].string(), '\\').back();
						if (ImGui::SmallButton(dirName.c_str()))
						{
							//change dirctory on click
							m_History.SwitchTo(m_Dirs[i]);
							m_CurrentPath = *m_History.GetCurrentFolder();
							m_ForceRescan = true;
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
						ImGui::BeginGroup();
						if (ImGui::SmallButton(m_Files[i].filename().string().c_str()))
						{
							//open file on click
							ViewerManager::OpenViewer(m_Files[i]);
						}
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("Asset", &m_Files[i], sizeof(std::filesystem::path));
							ImGui::Text(m_Files[i].filename().string().c_str());
							ImGui::EndDragDropSource();
						}
						ImGui::EndGroup();
					}
				}

				ImGui::PopID();
			}
			ImGui::EndChild();
		}
	}
	ImGui::End();
}